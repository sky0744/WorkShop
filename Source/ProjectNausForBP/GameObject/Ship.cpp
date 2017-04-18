// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ProjectNausForBP.h"
#include "Ship.h"

AShip::AShip()
{
	objectMesh->SetCanEverAffectNavigation(true);
	objectMesh->SetEnableGravity(false);
	objectMesh->SetSimulatePhysics(true);
	objectMesh->BodyInstance.MassScale = 100.0f;
	objectMesh->BodyInstance.LinearDamping = 50.0f;
	objectMesh->BodyInstance.AngularDamping = 5000.0f;
	objectMesh->BodyInstance.bLockZTranslation = true;
	objectMesh->BodyInstance.bLockXRotation = true;
	objectMesh->BodyInstance.bLockYRotation = true;
	objectMesh->Mobility = EComponentMobility::Movable;
	RootComponent = objectMesh;

	objectMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("ObjectMovement"));
	objectMovement->SetPlaneConstraintEnabled(true);
	objectMovement->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);
	
	AIControllerClass = ASpaceAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0f;
}

#pragma region Event Calls
void AShip::BeginPlay()
{
	Super::BeginPlay();
	traceParams = FCollisionQueryParams(FName("PathFind"), true, this);

	checkTime = 0.0f;
	if (GetWorld() && UGameplayStatics::GetGameState(GetWorld()))
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(false);
	UE_LOG(LogClass, Log, TEXT("[Info][Ship][Begin] Spawn Finish!"));
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	tempDeltaTime = DeltaTime;
	checkTime += DeltaTime;
	if (checkTime > 0.5f) {
		checkTime = 0.0f;

		currentShield = FMath::Clamp(currentShield + rechargeShield * 0.5f, 0.0f, maxShield);
		currentArmor = FMath::Clamp(currentArmor + repairArmor * 0.5f, 0.0f, maxArmor);
		currentHull = FMath::Clamp(currentHull + repairHull * 0.5f, 0.0f, maxHull);
		currentPower = FMath::Clamp(currentPower + rechargePower * 0.5f, 0.0f, maxPower);

		CheckPath();
		ModuleCheck();
	}

	switch (behaviorState) {
	case BehaviorState::Idle:
		break;
	case BehaviorState::Move:
		if (MoveDistanceCheck()) {
			behaviorState = BehaviorState::Idle;
			bIsStraightMove = true;
			moveTargetVector = GetActorLocation() + GetActorForwardVector() * 1000.0f;
		}
		break;
	case BehaviorState::Docking:
		if (MoveDistanceCheck()) {
			bIsStraightMove = true;
			moveTargetVector = Cast<AActor>(targetStructure.GetObjectRef())->GetActorForwardVector() * 1000.0f + GetActorLocation();
			behaviorState = BehaviorState::Docked;
			Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->SetDockedStructure(targetStructure);
			Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OnUIStationButton();
		}
		break;
	case BehaviorState::Warping:
		break;
	default:
		break;
	}
	RotateCheck();
	Movement();
}

float AShip::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	if (!DamageCauser->IsA(ASpaceObject::StaticClass()))
		return 0.0f;
	
	float remainDamage = DamageAmount * FMath::FRandRange(0.85f, 1.15f);

	float effectShieldDamage = 0.0f;
	float effectArmorDamage = 0.0f;
	float effectHullDamage = 0.0f;
	bool isCritical = false;

	FHitResult hitResult;
	FVector hitDirect;
	DamageEvent.GetBestHitInfo(this, DamageCauser, hitResult, hitDirect);
	hitDirect = hitResult.ImpactPoint - GetActorLocation();
	hitDirect.Normalize();

	ASpaceObject* aObj = Cast<ASpaceObject>(DamageCauser);

	UE_LOG(LogClass, Log, TEXT("[Info][Ship][Damaged] %f %f %f"), currentShield, currentArmor, currentHull);
	if (FVector::DotProduct(GetActorForwardVector(), hitDirect) > 0.95f) {
		remainDamage *= 2.0f;
		isCritical = true;
	}
	else if (FVector::DotProduct(GetActorForwardVector(), hitDirect) < -0.95f) {
		remainDamage *= 3.0f;
		isCritical = true;
	}

	//remainDamage = sDefShield.GetValue();
	if (currentShield > remainDamage) {
		effectShieldDamage = remainDamage;
		currentShield -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectShieldDamage = currentShield;
		remainDamage -= effectShieldDamage;
		currentShield = 0.0f;
	}

	if (currentArmor > remainDamage) {
		effectArmorDamage = remainDamage;
		currentArmor -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectArmorDamage = currentArmor;
		remainDamage -= effectArmorDamage;
		currentArmor = 0.0f;
	}

	if (currentHull > remainDamage) {
		effectHullDamage = remainDamage;
		currentHull -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectHullDamage = currentHull;
		currentHull = 0.0f;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, this->GetName() + " is Die!");
		Destroy();

		if (aObj->GetFaction() == Faction::Player && bounty > 0.0f)
			Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->ChangeCredit(bounty);
		
		/*Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->factionRelation[(uint8)faction * 12 + (uint8)aObj->GetFaction()] -= strategyPoint * 0.00005f;
		
		USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
		if (_tempInstance == nullptr)
			return effectShieldDamage + effectArmorDamage + effectHullDamage;
		FNPCData _tempNpcShipData = _tempInstance->GetNPCData(npcShipID);

		for (int index = 0; index < cargo.Num(); index++) 
			cargo[index].itemAmount = FMath::TruncToInt(cargo[index].itemAmount * FMath::RandRange(0.0f, 1.0f));

		int temp = FMath::Min3(_tempNpcShipData.dropItems.Num(), _tempNpcShipData.dropChance.Num(), _tempNpcShipData.dropRandomAmount.Num());
		cargo.Reserve(temp + cargo.Num());
		for (int index = 0; index < temp; index++) {
			if (_tempNpcShipData.dropChance[index] < FMath::RandRange(0.0f, 100.0f))
				continue;
			cargo.Emplace(FItem(_tempNpcShipData.cargoItems[index].itemID, FMath::Max(0, _tempNpcShipData.cargoItems[index].itemAmount - FMath::RandRange(_tempNpcShipData.havingRandomAmount[index], _tempNpcShipData.havingRandomAmount[index]))));
		}
		cargo.Shrink();
		*/
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Ship][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"), *this->GetName(), *DamageEvent.DamageTypeClass->GetName(), remainDamage, *DamageCauser->GetName(), effectShieldDamage, effectArmorDamage, effectHullDamage, isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return effectShieldDamage + effectArmorDamage + effectHullDamage;
}

void AShip::BeginDestroy() {
	if(GetWorld() && UGameplayStatics::GetGameState(GetWorld()))
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(true);

	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int AShip::GetObjectID() {
	return npcShipID;
}

ObjectType AShip::GetObjectType() {
	return ObjectType::Ship;
}

Faction AShip::GetFaction() {
	return faction;
}

void AShip::SetFaction(Faction setFaction) {
	faction = setFaction;
}

BehaviorState AShip::GetBehaviorState() {
	return behaviorState;
}

bool AShip::InitObject(int npcID) {
	if (isInited == true || npcID < 0)
		return false;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (_tempInstance == nullptr)
		return false;

	FNPCData _tempNpcShipData = _tempInstance->GetNPCData(npcID);
	FShipData _tempShipData = _tempInstance->GetShipData(_tempNpcShipData.ShipID);
	FItemData _tempModuleData;

	npcShipID = _tempNpcShipData.NPCID;
	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempShipData.MeshPath.ToString()));
	objectMesh->SetStaticMesh(newMesh);

	shipClass = _tempShipData.Shipclass;
	faction = _tempNpcShipData.faction;
	behaviorType = _tempNpcShipData.behaviorType;
	lengthRader = FMath::Clamp(_tempShipData.lengthRader, 10.0f, 100000.0f); ;

	int tempModuleSlotNumber = FMath::Min(_tempShipData.SlotTarget, _tempNpcShipData.EquipedSlotTarget.Num());
	slotTargetModule.Empty();
	for (int index = 0; index < tempModuleSlotNumber; index++) {
		_tempModuleData = _tempInstance->GetItemData(_tempNpcShipData.EquipedSlotTarget[index]);
		if (_tempModuleData.Type == ItemType::TargetModule) {
			slotTargetModule[index].moduleID = _tempModuleData.ItemID;
			slotTargetModule[index].moduleType = _tempModuleData.ModuleType;
			slotTargetModule[index].maxCooltime = _tempModuleData.MaxCooltime;
			slotTargetModule[index].maxUsagePower = _tempModuleData.MaxUsagePower;
			slotTargetModule[index].incrementUsagePower = _tempModuleData.IncrementUsagePower;
			slotTargetModule[index].decrementUsagePower = _tempModuleData.DecrementUsagePower;

			slotTargetModule[index].damageMultiple = _tempModuleData.DamageMultiple;
			slotTargetModule[index].launchSpeedMultiple = _tempModuleData.LaunchSpeedMultiple;
			slotTargetModule[index].accaucy = _tempModuleData.Accaucy;
			slotTargetModule[index].ammoLifeSpanBonus = _tempModuleData.AmmoLifeSpanBonus;

			slotTargetModule[index].ammo = FItem(_tempModuleData.UsageAmmo, 0);
			slotTargetModule[index].ammoCapacity = _tempModuleData.AmmoCapacity;
		}
	}

	if (&_tempNpcShipData == nullptr || &_tempShipData == nullptr)
		return false;

	lengthToLongAsix = _tempShipData.lengthToLongAsix;
	strategyPoint = _tempNpcShipData.strategyPoint;
	bounty = _tempNpcShipData.npcBounty;

	maxShield = FMath::Clamp(_tempShipData.Shield, 10.0f, 1000000.0f);
	rechargeShield = FMath::Clamp(_tempShipData.RechargeShield, 0.0f, 500.0f);
	defShield = FMath::Clamp(_tempShipData.DefShield, 0.0f, 1000.0f);
	currentShield = maxShield;

	maxArmor = FMath::Clamp(_tempShipData.Armor, 10.0f, 1000000.0f);
	repairArmor = FMath::Clamp(_tempShipData.RepairArmor, 0.0f, 500.0f);
	defArmor = FMath::Clamp(_tempShipData.DefArmor, 0.0f, 1000.0f);
	currentArmor = maxArmor;

	maxHull = FMath::Clamp(_tempShipData.Hull, 10.0f, 1000000.0f);
	repairHull = FMath::Clamp(_tempShipData.RepairHull, 0.0f, 500.0f);
	defHull = FMath::Clamp(_tempShipData.DefHull, 0.0f, 1000.0f);
	currentHull = maxHull;

	maxPower = FMath::Clamp(_tempShipData.Power, 10.0f, 1000000.0f);
	rechargePower = FMath::Clamp(_tempShipData.RechargePower, 0.0f, 5000.0f);
	currentPower = maxPower;

	maxSpeed = FMath::Clamp(_tempShipData.MaxSpeed, 0.0f, 10000.0f);
	minAcceleration = FMath::Clamp(_tempShipData.MinAcceleration, 0.0f, 10000.0f);
	maxAcceleration = FMath::Clamp(_tempShipData.MaxAcceleration, 0.0f, 10000.0f);
	startAccelAngle = FMath::Clamp(_tempShipData.MaxAcceleration, 0.0f, 180.0f);

	maxRotateRate = FMath::Clamp(_tempShipData.MaxRotateRate, 0.0f, 90.0f);
	rotateAcceleration = FMath::Clamp(_tempShipData.MaxRotateRate, 0.0f, 90.0f);
	rotateDeceleration = FMath::Clamp(_tempShipData.MaxRotateRate, 0.0f, 90.0f);

	for (FBonusStat& bonusStat : _tempShipData.bonusStats) {

		switch (bonusStat.bonusStatType) {
		case BonusStatType::BonusMaxShield:
			maxShield = FMath::Clamp(maxShield * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 10.0f, 1000000.0f);
			break;
		case BonusStatType::BonusRechargeShield:
			rechargeShield = FMath::Clamp(rechargeShield * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 500.0f);
			break;
		case BonusStatType::BonusDefShield:
			defShield = FMath::Clamp(defShield * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 1000.0f);
			break;
		case BonusStatType::BonusMaxArmor:
			maxArmor = FMath::Clamp(maxArmor * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 10.0f, 1000000.0f);
			break;
		case BonusStatType::BonusRepaireArmor:
			repairArmor = FMath::Clamp(repairArmor * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 500.0f);
			break;
		case BonusStatType::BonusDefArmor:
			defArmor = FMath::Clamp(defArmor * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 1000.0f);
			break;
		case BonusStatType::BonusMaxHull:
			maxHull = FMath::Clamp(maxHull * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 10.0f, 1000000.0f);
			break;
		case BonusStatType::BonusRepaireHull:
			repairHull = FMath::Clamp(repairHull * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 500.0f);
			break;
		case BonusStatType::BonusDefHull:
			defHull = FMath::Clamp(defHull * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 1000.0f);
			break;
		case BonusStatType::BonusMaxPower:
			maxPower = FMath::Clamp(maxPower * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 10.0f, 1000000.0f);
			break;
		case BonusStatType::BonusRechargePower:
			rechargePower = FMath::Clamp(rechargePower * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 500.0f);
			break;
		case BonusStatType::BonusMobilitySpeed:
			maxSpeed = FMath::Clamp(maxSpeed * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 10000.0f);
			break;
		case BonusStatType::BonusMobilityAcceleration:
			maxAcceleration = FMath::Clamp(maxAcceleration * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 1000.0f);
			minAcceleration = FMath::Clamp(minAcceleration * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 1000.0f);
			break;
		case BonusStatType::BonusMobilityRotation:
			maxRotateRate = FMath::Clamp(maxRotateRate * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 90.0f);
			break;
		case BonusStatType::BonusMobilityRotateAcceleration:
			rotateAcceleration = FMath::Clamp(rotateAcceleration * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 90.0f);
			rotateDeceleration = FMath::Clamp(rotateDeceleration * (1.0f + FMath::Clamp(bonusStat.bonusStat, 0.0f, 5.0f)), 0.0f, 90.0f);
			break;
		default:
			for (int index = 0; index < slotTargetModule.Num(); index++) {
				if (slotTargetModule[index].moduleID > 0) {
					switch (slotTargetModule[index].moduleType) {
					case ModuleType::Beam:
						if (bonusStat.bonusStatType == BonusStatType::BonusBeamDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusBeamPower)
							slotTargetModule[index].maxUsagePower *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusBeamCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusBeamAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusBeamRange)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						break;
					case ModuleType::Cannon:
						if (bonusStat.bonusStatType == BonusStatType::BonusCannonDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusCannonCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusCannonAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusCannonLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusCannonLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						break;
					case ModuleType::Railgun:
						if (bonusStat.bonusStatType == BonusStatType::BonusRailGunDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusRailGunPower)
							slotTargetModule[index].maxUsagePower *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusRailGunCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusRailGunAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusRailGunLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusRailGunLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						break;
					case ModuleType::MissileLauncher:
						if (bonusStat.bonusStatType == BonusStatType::BonusMissileDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusMissileCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusMissileAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusMissileLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusMissileLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						break;
					case ModuleType::MinerLaser:
						if (bonusStat.bonusStatType == BonusStatType::BonusMiningAmount)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusMiningPower)
							slotTargetModule[index].maxUsagePower *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusMiningCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.bonusStat, 0.0f, 5.0f);
						else if (bonusStat.bonusStatType == BonusStatType::BonusMiningRange)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.bonusStat, 1.0f, 5.0f);
						break;
					default:
						break;
					}
				}
			}
			break;
		}
	}
	AddActorWorldOffset(FVector(0.0f, 0.0f, 0.0f));
	return true;
}

bool AShip::LoadBaseObject(float shield, float armor, float hull, float power) {
	
	return true;
}

float AShip::GetValue(GetStatType statType) {
	float _value;
	
	switch (statType) {
	case GetStatType::halfLength:
		_value = lengthToLongAsix * 0.5f;
		break;
	case GetStatType::raderDistance:
		_value = lengthRader;
		break;
	case GetStatType::engageDistance:
		_value = lengthRader;
		break;
	case GetStatType::maxShield:
		_value = maxShield;
		break;
	case GetStatType::rechargeShield:
		_value = rechargeShield;
		break;
	case GetStatType::currentShield:
		_value = currentShield;
		break;
	case GetStatType::defShield:
		_value = defShield;
		break;

	case GetStatType::maxArmor:
		_value = maxArmor;
		break;
	case GetStatType::currentArmor:
		_value = currentArmor;
		break;
	case GetStatType::defArmor:
		_value = defArmor;
		break;

	case GetStatType::maxHull:
		_value = maxHull;
		break;
	case GetStatType::currentHull:
		_value = currentHull;
		break;
	case GetStatType::defHull:
		_value = defHull;
		break;

	case GetStatType::maxPower:
		_value = maxPower;
		break;
	case GetStatType::rechargePower:
		_value = rechargePower;
		break;
	case GetStatType::currentPower:
		_value = currentPower;
		break;

	case GetStatType::maxSpeed:
		_value = maxSpeed;
		break;
	case GetStatType::targetSpeed:
		_value = targetSpeed;
		break;
	case GetStatType::currentSpeed:
		_value = currentSpeed;
		break;

	case GetStatType::maxAcceleration:
		_value = maxAcceleration;
		break;
	case GetStatType::minAcceleration:
		_value = minAcceleration;
		break;
	case GetStatType::currentAcceleration:
		_value = accelerationFactor;
		break;

	case GetStatType::maxRotateRate:
		_value = maxRotateRate;
		break;
	case GetStatType::currentRotateRate:
		_value = rotateRateFactor;
		break;
	default:
		_value = -1;
		break;
	}
	return _value;
}

void AShip::GetRepaired(GetStatType statType, float repairValue) {

	repairValue = FMath::Clamp(repairValue, 0.0f, 500.0f);
	switch (statType) {
	case GetStatType::currentShield:
		currentShield = FMath::Clamp(currentShield + repairValue, 0.0f, maxShield);
		break;
	case GetStatType::currentArmor:
		currentArmor = FMath::Clamp(currentArmor + repairValue, 0.0f, maxArmor);
		break;
	case GetStatType::currentHull:
		currentHull = FMath::Clamp(currentHull + repairValue, 0.0f, maxHull);
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region Interface Implementing : ICommandable
void AShip::CommandStop() {

	behaviorState = BehaviorState::Idle;
	setedTargetSpeed = 0.0f;
	targetRotateRateFactor = 0.0f;
	bIsStraightMove = false;
}

bool AShip::CommandMoveToPosition(FVector position) {
	if (CheckCanBehavior() == true) {
		
		moveTargetVector = position;
		targetObject = nullptr;
		bIsStraightMove = true;
		RequestPathUpdate();
		behaviorState = BehaviorState::Move;
		return true;
	}
	else return false;
}

bool AShip::CommandMoveToTarget(ASpaceObject* target) {
	if (CheckCanBehavior() == true) {
		targetObject = target;
		bIsStraightMove = true;
		RequestPathUpdate();
		behaviorState = BehaviorState::Move;
		return true;
	}
	else return false;
}

bool AShip::CommandAttack(ASpaceObject* target) {
	if (CheckCanBehavior() == true) {
		targetObject = target;
		behaviorState = BehaviorState::Battle;
		return true;
	}
	else return false;
}

bool AShip::CommandMining(TScriptInterface<ICollectable> target) {
	if (CheckCanBehavior() == true) {
		targetCollect = target;
		behaviorState = BehaviorState::Mining;
		return true;
	}
	else return false;
}

bool AShip::CommandRepair(ASpaceObject* target) {
	if (CheckCanBehavior() == true) {
		TArray<bool> isRepairableModuleInSlot;
		//for(int index = 0; slotTargetModule.Num(); index++)
		//	slotTargetModule[index]
		switch (target->GetObjectType()) {

		case ObjectType::Station:
		case ObjectType::Ship:
		case ObjectType::Drone:
		case ObjectType::Gate:
			targetObject = target;
		}
		return true;
	}
	else return false;
}

bool AShip::CommandJump(TScriptInterface<IStructureable> target) {
	if (CheckCanBehavior() == true) {
		Destroy();
		return true;
	}
	else return false;
}

bool AShip::CommandWarp(FVector location) {
	if (CheckCanBehavior() == true) {
		SetActorLocation(location, false, nullptr, ETeleportType::TeleportPhysics);
		behaviorState = BehaviorState::Idle;
		return true;
	}
	else return false;
}

bool AShip::CommandDock(TScriptInterface<IStructureable> target) {
	if (CheckCanBehavior() == true && target.GetObjectRef()->IsA(ASpaceObject::StaticClass())) {
		if (target->RequestedDock(Faction::Player)) {
			targetStructure = target;
			targetObject = Cast<ASpaceObject>(target.GetObjectRef());
			bIsStraightMove = true;
			RequestPathUpdate();
			behaviorState = BehaviorState::Docking;
			return true;
		}
		else return false;
	}
	else return false;
}

bool AShip::CommandUndock() {
	if (behaviorState == BehaviorState::Docked) {
		bIsStraightMove = true;
		targetObject = nullptr;
		behaviorState = BehaviorState::Idle;
		return true;
	}
	else return false;
}

bool AShip::CommandLaunch(TArray<int> baySlot) {
	return false;
}

/*
* 타게팅 모듈의 동작을 제어. 플레이어 전용.
* @param slotIndex - 타게팅 모듈 슬롯 번호.
* @return 처리 후의 모듈 동작 상태.
*/
bool AShip::CommandToggleTargetModule(int slotIndex, ASpaceObject* target) {
	if (target == nullptr || target == this) 
		return false;

	//타게팅 모듈에 한해서만( < ModuleType::ShieldGenerator ) 함수 처리
	if (slotIndex < slotTargetModule.Num() && slotTargetModule[slotIndex].moduleType < ModuleType::ShieldGenerator) {
		//현재 모듈이 활성화되어 있을 경우 -> 작동 중지 예약, 현재 켜져있는 상태임을 리턴
		if (slotTargetModule[slotIndex].moduleState != ModuleState::NotActivate) {
			slotTargetModule[slotIndex].isBookedForOff = true;
			return true;
		}

		else {
			//현재 꺼져있는 상태
			switch (slotTargetModule[slotIndex].moduleType) {
			case ModuleType::Beam:
			case ModuleType::TractorBeam:
			case ModuleType::MinerLaser:
				//모듈이 빔 계열인 경우
				if (target == nullptr)
					return false;
				slotTargetModule[slotIndex].moduleState = ModuleState::Activate;
				slotTargetModule[slotIndex].isBookedForOff = false;
				slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
				targetingObject[slotIndex] = target;
				return true;
				break;
			case ModuleType::Cannon:
			case ModuleType::Railgun:
			case ModuleType::MissileLauncher:
				//모듈이 실탄 계열인 경우, 모듈을 활성화하기 이전에 ammo 상태를 확인
				if (slotTargetModule[slotIndex].ammo.itemAmount < 1) {
					USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
					AUserState* userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
					FItemData _tempModuleData = _tempInstance->GetItemData(slotTargetModule[slotIndex].moduleID);
					TArray<FItem> _tempItemSlot;
					int _findSlot;

					userState->GetUserDataItem(_tempItemSlot);
					_findSlot = USafeENGINE::FindItemSlot(_tempItemSlot, FItem(_tempModuleData.UsageAmmo, 0));

					//ammo를 카고 리스트에서 찾았음.
					if (_findSlot > -1) {
						slotTargetModule[slotIndex].moduleState = ModuleState::ReloadAmmo;
						targetingObject[slotIndex] = target;
						slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
					}
				}
				else if (target != nullptr) {
					slotTargetModule[slotIndex].moduleState = ModuleState::Activate;
					targetingObject[slotIndex] = target;
					slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
				}
				slotTargetModule[slotIndex].isBookedForOff = false;
				return true;
				break;
			default:
				return false;
				break;
			}
		}
	}
	for (int index = 0; index < slotTargetModule.Num(); index++)
		slotTargetModule[index].moduleState = ModuleState::NotActivate;
	return false;
}

bool AShip::CommandToggleActiveModule(int slotIndex) {

	return false;
}
#pragma endregion

#pragma region Functions

BehaviorType AShip::GetBehaviorType() {
	return behaviorType;
}

ShipClass AShip::GetShipClass() {
	return shipClass;
}

void AShip::GetDockedStructure(TScriptInterface<IStructureable>& getStructure) {
	getStructure = targetStructure;
}

bool AShip::MoveDistanceCheck() {

	FRotator _targetRotate;
	FVector _realMoveFactor;
	float _distance;

	if (targetObject != nullptr) {
		moveTargetVector = targetObject->GetActorLocation();
		_realMoveFactor = moveTargetVector - GetActorLocation();
		_distance = FVector::Dist(GetActorLocation(), moveTargetVector) - lengthToLongAsix - targetObject->GetValue(GetStatType::halfLength);
		_targetRotate = _realMoveFactor.Rotation() - GetActorRotation();
	}
	else {
		_realMoveFactor = moveTargetVector - GetActorLocation();
		_distance = FVector::Dist(FVector::ZeroVector, _realMoveFactor);
		_targetRotate = _realMoveFactor.Rotation() - GetActorRotation();
	}


	//checks distance and Angle For start Acceleration.
	if (_distance > (FMath::Pow(currentSpeed, 2) / FMath::Clamp(minAcceleration * accelerationFactor * 2.0f, 1.0f, 9999.0f) + 5.0f)) {
		if (FMath::Abs(_targetRotate.Yaw) < startAccelAngle)
			setedTargetSpeed = targetSpeed;
		else
			setedTargetSpeed = 0.0f;
	}
	else setedTargetSpeed = 0.0f;

	//arrive to Destination. use upper of Nyquist Rate for high precision.
	if (_distance < currentSpeed * tempDeltaTime * 50.0f) {
		setedTargetSpeed = 0.0f;
		return true;
	}

	//arrive to Destination not yet.
	return false;
}

void AShip::RotateCheck() {

	FRotator _targetRotate;
	FVector _realMoveFactor;
	float resultOuter;

	if (targetObject != nullptr) {
		_realMoveFactor = targetObject->GetActorLocation() - GetActorLocation();
		_realMoveFactor.Normalize();
		_targetRotate = _realMoveFactor.Rotation() - GetActorRotation();
	}
	else {
		_realMoveFactor = moveTargetVector - GetActorLocation();
		_targetRotate = _realMoveFactor.Rotation() - GetActorRotation();
	}
	resultOuter = FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(GetActorForwardVector(), _realMoveFactor));
	_targetRotate.Normalize();

	if (resultOuter > 0.01f) {
		if (FMath::Abs(_targetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(rotateDeceleration * rotateRateFactor * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = maxRotateRate * rotateRateFactor;
		else
			targetRotateRateFactor = 0.0f;
	}
	//left on side, rotate left
	else if (resultOuter < -0.01f) {
		if (FMath::Abs(_targetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(rotateDeceleration * rotateRateFactor * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = maxRotateRate * rotateRateFactor;
		else
			targetRotateRateFactor = 0.0f;
	}
	else targetRotateRateFactor = 0.0f;
}

void AShip::Movement() {
	if (targetRotateRateFactor > 0.0f) {
		if (realRotateRateFactor >= 0.0f) {
			if (targetRotateRateFactor > realRotateRateFactor)
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * rotateAcceleration * tempDeltaTime, 0.0f, targetRotateRateFactor);
			else
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * rotateDeceleration * tempDeltaTime, targetRotateRateFactor, maxRotateRate * rotateRateFactor);
		}
		else {
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * rotateDeceleration * tempDeltaTime, -maxRotateRate * rotateRateFactor, 0.0f);
		}
	}
	else if (targetRotateRateFactor < 0.0f) {
		if (realRotateRateFactor <= 0.0f) {
			if (targetRotateRateFactor > realRotateRateFactor)
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * rotateDeceleration * tempDeltaTime, -maxRotateRate * rotateRateFactor, targetRotateRateFactor);
			else
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * rotateAcceleration * tempDeltaTime, targetRotateRateFactor, 0.0f);
		}
		else {
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * rotateDeceleration * tempDeltaTime, 0.0f, maxRotateRate * rotateRateFactor);
		}
	}
	else {
		if (realRotateRateFactor < 0.0f)
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * rotateDeceleration * tempDeltaTime, -maxRotateRate * rotateRateFactor, 0.0f);
		else
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * rotateDeceleration * tempDeltaTime, 0.0f, maxRotateRate * rotateRateFactor);
	}

	if (currentSpeed < setedTargetSpeed)
		currentSpeed = FMath::Clamp(currentSpeed + accelerationFactor * maxAcceleration * tempDeltaTime, 0.0f, setedTargetSpeed);
	else if (currentSpeed > setedTargetSpeed)
		currentSpeed = FMath::Clamp(currentSpeed - accelerationFactor * minAcceleration * tempDeltaTime, 0.0f, maxSpeed );

	AddActorLocalRotation(FRotator(0.0f, realRotateRateFactor, 0.0f) * tempDeltaTime);
	AddActorWorldOffset(GetActorForwardVector() * currentSpeed * tempDeltaTime, true);
}

void AShip::ModuleCheck() {
	/*
	for (int index = 0; index < slotTargetModule.Num(); index++) {
		//에너지가 부족할 경우 모든 모듈의 동작을 중지 예약.
		if (sCurrentPower < 5.0f)
			slotTargetModule[index].isBookedForOff = true;

		//모듈이 켜져있을 경우 power 소모량 증가 및 쿨타임 지속 감소
		if (slotTargetModule[index].moduleState != ModuleState::NotActivate) {
			slotTargetModule[index].currentUsagePower = FMath::Clamp(slotTargetModule[index].currentUsagePower + slotTargetModule[index].incrementUsagePower * 0.5f,
				0.0f, slotTargetModule[index].maxUsagePower);
			slotTargetModule[index].remainCooltime = FMath::Clamp(slotTargetModule[index].remainCooltime - 0.5f, 0.0f, FMath::Max(1.0f, slotTargetModule[index].maxCooltime));

			//쿨타임 완료시 행동 실시
			if (slotTargetModule[index].remainCooltime <= 0.0f) {
				switch (slotTargetModule[index].moduleState) {
				case ModuleState::Activate:
				case ModuleState::Overload:
					if (targetingObject[index] != nullptr && targetingObject[index] != this) {
						//목표 지점 및 방향 계산
						FVector _targetedLocation = targetingObject[index]->GetActorLocation();
						FVector _targetedDirect = _targetedLocation - GetActorLocation();
						_targetedDirect.Normalize();
						FRotator _targetedRotation = _targetedDirect.Rotation();
						FTransform _spawnedTransform = FTransform(_targetedRotation, GetActorLocation() + _targetedDirect * lengthToLongAsix);

						//빔계열 모듈의 경우
						if (slotTargetModule[index].moduleType == ModuleType::Beam ||
							slotTargetModule[index].moduleType == ModuleType::MinerLaser) {

							ABeam* _beam = Cast<ABeam>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ABeam::StaticClass()
								, _spawnedTransform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
							if (_beam == nullptr)
								return;
							UGameplayStatics::FinishSpawningActor(_beam, _spawnedTransform);

							if (slotTargetModule[index].moduleType == ModuleType::Beam)
								_beam->SetBeamProperty(this, _beam->GetActorLocation() + _targetedDirect * slotTargetModule[index].launchSpeedMultiple,
									true, slotTargetModule[index].damageMultiple, 1.0f);
							else
								_beam->SetBeamProperty(this, _beam->GetActorLocation() + _targetedDirect * slotTargetModule[index].launchSpeedMultiple,
									true, slotTargetModule[index].damageMultiple, FMath::Max(1.0f, slotTargetModule[index].maxCooltime));
						}
						//탄도 무기류의 경우
						else if (slotTargetModule[index].moduleType == ModuleType::Cannon ||
							slotTargetModule[index].moduleType == ModuleType::Railgun ||
							slotTargetModule[index].moduleType == ModuleType::MissileLauncher) {

							if (slotTargetModule[index].ammo.itemAmount < 1) {
								slotTargetModule[index].moduleState = ModuleState::ReloadAmmo;
								slotTargetModule[index].isBookedForOff = false;
							}
							else {
								slotTargetModule[index].ammo.itemAmount--;

								AProjectiles* _projectile = Cast<AProjectiles>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AProjectiles::StaticClass(),
									_spawnedTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
								if (_projectile == nullptr)
									return;
								UGameplayStatics::FinishSpawningActor(_projectile, _spawnedTransform);

								ASpaceObject* selfObject = this;

								switch (slotTargetModule[index].moduleType) {
								case ModuleType::Cannon:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, selfObject,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, bonusCannonLifeTime);
									break;
								case ModuleType::Railgun:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, selfObject,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, bonusRailGunLifeTime);
									break;
								case ModuleType::MissileLauncher:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, selfObject,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, bonusMissileLifeTime, targetingObject[index]);
									break;
								}
							}
						}
					}
					else {
						UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][Tick] Target Module %d - Off."), index);
						slotTargetModule[index].moduleState = ModuleState::NotActivate;
						slotTargetModule[index].isBookedForOff = false;
					}
					break;
				case ModuleState::ReloadAmmo:
					if (slotTargetModule[index].moduleType == ModuleType::Cannon ||
						slotTargetModule[index].moduleType == ModuleType::Railgun ||
						slotTargetModule[index].moduleType == ModuleType::MissileLauncher) {

						USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
						AUserState* userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);

						FItemData _tempModuleData = _tempInstance->GetItemData(slotTargetModule[index].moduleID);
						TArray<FItem> _tempItemSlot;
						int _reloadedAmount;
						int _findSlot;

						userState->GetUserDataItem(_tempItemSlot);
						_findSlot = USafeENGINE::FindItemSlot(_tempItemSlot, FItem(_tempModuleData.UsageAmmo, 0));


						//ammo를 찾지 못하였음. 재장전 및 모듈 동작 실시하지 않음.
						if (_findSlot < 0)
							slotTargetModule[index].moduleState = ModuleState::NotActivate;

						//ammo를 찾음. 재장전 실시. 모듈 동작은 재장전이 완료되면 자동으로 시작.
						else {
							_reloadedAmount = FMath::Min(_tempItemSlot[_findSlot].itemAmount, slotTargetModule[index].ammoCapacity - slotTargetModule[index].ammo.itemAmount);
							if (userState->DropPlayerCargo(FItem(_tempItemSlot[_findSlot].itemID, _reloadedAmount))) {
								UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Reload Start!"));
								slotTargetModule[index].ammo.itemAmount = _reloadedAmount;
								if (targetingObject[index] != nullptr) {
									slotTargetModule[index].moduleState = ModuleState::Activate;
									UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Reload Finish! and Target On!"));
								}
								else {
									slotTargetModule[index].moduleState = ModuleState::NotActivate;
									UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Reload Finish!"));
								}
							}
							else {
								//ammo를 찾았으나 장전에 실패.
								UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Reload Fail!"));
								slotTargetModule[index].moduleState = ModuleState::NotActivate;
							}
						}

						slotTargetModule[index].isBookedForOff = false;
					}
					break;
				default:
					break;
				}

				slotTargetModule[index].remainCooltime = FMath::Max(1.0f, slotTargetModule[index].maxCooltime);
				//모듈 동작 중지 예약이 활성화되어 있다면 동작 중지.
				if (slotTargetModule[index].isBookedForOff) {
					UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][Tick] Target Module %d - Off."), index);
					slotTargetModule[index].moduleState = ModuleState::NotActivate;
					slotTargetModule[index].isBookedForOff = false;
				}
			}
		}
		//모듈이 꺼져있을 경우 power 소모량 감소
		else
			slotTargetModule[index].currentUsagePower = FMath::Clamp(slotTargetModule[index].currentUsagePower - slotTargetModule[index].decrementUsagePower * 0.5f,
				0.0f, slotTargetModule[index].maxUsagePower);
		moduleConsumptPower += slotTargetModule[index].currentUsagePower;
	}*/
}

bool AShip::CheckCanBehavior() {
	switch (behaviorState)
	{
	case BehaviorState::Docked:
	case BehaviorState::Warping:
		return false;
	default:
		return true;
	}
}
#pragma endregion

#pragma region Path Finder
/*
* Called by GameMode - (Space)'s Broadcast In Tick. interval is 0.5 seconds.
* Check 1 : Check front obstacle detect, Length : Object's LongAsix's Length * 2.0f + 1000.0f.
* Check 2 : Check obstacle detection to Target, Length : to Target Distance.
* if(Check1 or Check2 is true, request Path Update.)
*/
void AShip::CheckPath() {
	FVector _forTargetDirectionVector = moveTargetVector - GetActorLocation();
	_forTargetDirectionVector.Normalize();

	if (!CheckCanBehavior() || behaviorState == BehaviorState::Idle)
		return;

	bool bMoveTargetHited = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), GetActorLocation() + _forTargetDirectionVector * (lengthToLongAsix + 10.0f)
		, moveTargetVector, FVector(0.0f, lengthToLongAsix * 0.5f + 10.0f, 50.0f), _forTargetDirectionVector.Rotation()
		, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, frontTraceResult, true);
	bool bFrontHited = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), GetActorLocation() + GetActorForwardVector() * (lengthToLongAsix + 10.0f)
		, GetActorLocation() + GetActorForwardVector() * (lengthToLongAsix * 2.0f + 200.0f), FVector(0.0f, lengthToLongAsix * 0.5f + 10.0f, 50.0f), GetActorRotation()
		, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, frontTraceResult, true);

	bIsStraightMove = !bMoveTargetHited;
	if (!(!bMoveTargetHited || !bFrontHited))
		RequestPathUpdate();
}

/*
* Called by Movement Command or Check LineTrace of Obstacle when Hitted Cast.
* this is based on NavMesh. but, Movement logic is Custom.
*/
void AShip::RequestPathUpdate() {
	remainDistance = FVector::Dist(moveTargetVector, GetActorLocation());
	if (targetObject != nullptr) {
		moveTargetVector = targetObject->GetActorLocation();
		remainDistance = FVector::Dist(moveTargetVector, GetActorLocation()) - (lengthToLongAsix + targetObject->GetValue(GetStatType::halfLength));
		moveTargetVector.Normalize();
		moveTargetVector = moveTargetVector + moveTargetVector * remainDistance;
	}

	waypointData = UNavigationSystem::GetCurrent(GetWorld())->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), moveTargetVector);
	wayPoint = waypointData->PathPoints;
	if (!waypointData)
		return;
	bIsStraightMove = false;
	currentClosedPathIndex = 0;
}
#pragma endregion