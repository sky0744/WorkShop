// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "Ship.h"

AShip::AShip() {
	objectMesh->SetCanEverAffectNavigation(true);
	objectMesh->SetEnableGravity(false);
	objectMesh->SetSimulatePhysics(true);
	objectMesh->BodyInstance.LinearDamping = 500.0f;
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
	npcShipID = -1;
	isInited = false;

	slotTargetModule = TArray<FTargetModule>();
	targetingObject = TArray<ASpaceObject*>();
}

#pragma region Event Calls
void AShip::BeginPlay()
{
	Super::BeginPlay();
	traceParams = FCollisionQueryParams(FName("PathFind"), true, this);

	checkTime = 0.0f;
	UE_LOG(LogClass, Log, TEXT("[Info][Ship][Begin] Spawn Finish!"));
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	tempDeltaTime = DeltaTime;
	checkTime += DeltaTime;
	if (checkTime > _define_ModuleANDPathTick) {
		checkTime = 0.0f;

		currentShield = FMath::Clamp(currentShield + rechargeShield * _define_ModuleANDPathTick, 0.0f, maxShield);
		currentArmor = FMath::Clamp(currentArmor + repairArmor * _define_ModuleANDPathTick, 0.0f, maxArmor);
		currentHull = FMath::Clamp(currentHull + repairHull * _define_ModuleANDPathTick, 0.0f, maxHull);
		currentPower = FMath::Clamp(currentPower + rechargePower * _define_ModuleANDPathTick, 0.0f, maxPower);
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
	Faction _dealingFaction;

	if (DamageCauser->IsA(ABeam::StaticClass()))
		_dealingFaction = Cast<ABeam>(DamageCauser)->GetLaunchingFaction();
	else if (DamageCauser->IsA(AProjectiles::StaticClass()))
		_dealingFaction = Cast<AProjectiles>(DamageCauser)->GetLaunchingFaction();
	else
		return 0.0f;

	FHitResult _hitResult;
	FVector _hitDirect;
	DamageEvent.GetBestHitInfo(this, DamageCauser, _hitResult, _hitDirect);
	_hitDirect = _hitResult.ImpactPoint - GetActorLocation();
	_hitDirect.Normalize();

	float _remainDamage = DamageAmount * FMath::FRandRange(0.85f, 1.15f);
	float _effectShieldDamage = 0.0f;
	float _effectArmorDamage = 0.0f;
	float _effectHullDamage = 0.0f;
	float _effectReduceDamage = 0.0f;
	bool _isCritical = false;

	if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) > 0.95f) {
		_remainDamage *= 2.0f;
		_isCritical = true;
	} else if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) < -0.95f) {
		_remainDamage *= 3.0f;
		_isCritical = true;
	}

	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	if (USafeENGINE::IsValid(_spaceState) && _dealingFaction == Faction::Player)
		_spaceState->ApplyRelation(_dealingFaction, _remainDamage);

	/*
	*	데미지 경감 공식 : 경감률 = (def -1)^2 + 0.15,
	*	Def 범위 : -1000.0f ~ 1000.0f -> 최대 경감률 : -4.15f(역경감) ~ 0.15f
	*/
	if (currentShield > _remainDamage) {
		_effectReduceDamage = FMath::Pow((FMath::Clamp(defShield, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
		_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
		_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
		_effectShieldDamage = _remainDamage;
		currentShield -= _remainDamage;
		_remainDamage = 0.0f;
	} else {
		_effectShieldDamage = currentShield;
		_remainDamage -= _effectShieldDamage;
		currentShield = 0.0f;

		if (currentArmor > _remainDamage) {
			_effectReduceDamage = FMath::Pow((FMath::Clamp(defArmor, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
			_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
			_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
			_effectArmorDamage = _remainDamage;
			currentArmor -= _remainDamage;
			_remainDamage = 0.0f;
		} else {
			_effectArmorDamage = currentArmor;
			_remainDamage -= _effectArmorDamage;
			currentArmor = 0.0f;

			_effectReduceDamage = FMath::Pow((FMath::Clamp(defHull, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
			_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
			_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
			_effectHullDamage = _remainDamage;
			currentHull -= _remainDamage;
			_remainDamage = 0.0f;
		}
	}
	if (currentHull <= 0.0f) {
		_effectHullDamage = currentHull;
		currentHull = 0.0f;

		AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
		Peer _peerResult = Peer::Neutral;

		if (USafeENGINE::IsValid(_userState) && USafeENGINE::IsValid(_spaceState) && _dealingFaction == Faction::Player) {
			_peerResult = _spaceState->PeerIdentify(faction, _dealingFaction, true);
			//전략 포인트의 일부 가중치를 팩션 관계도에 반영
			_spaceState->ApplyRelation(_dealingFaction, strategicPoint, true);
			if (_dealingFaction == Faction::Player) {
				if (bounty > 0.0f)
					_userState->ChangeCredit(bounty);
				_userState->ChangeRenown(_peerResult, strategicPoint);
			}
		}
		//카고 드랍
		ACargoContainer* _cargoContanier = Cast<ACargoContainer>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ACargoContainer::StaticClass(),
			FTransform(this->GetActorRotation(), this->GetActorLocation()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
		if (_cargoContanier) {
			_cargoContanier->SetCargoFromData(ObjectType::Ship, npcShipID);
			UGameplayStatics::FinishSpawningActor(_cargoContanier, _cargoContanier->GetTransform());
		}
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Ship][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage,
		_isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void AShip::BeginDestroy() {
	if (GetWorld() && UGameplayStatics::GetGameState(GetWorld()) && UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass())) {
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(true);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->RemoveFromObjectList(this);
	}
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int AShip::GetObjectID() const {
	return npcShipID;
}

ObjectType AShip::GetObjectType() const {
	return ObjectType::Ship;
}

Faction AShip::GetFaction() const {
	return faction;
}

void AShip::SetFaction(const Faction setFaction) {
	faction = setFaction;
}

BehaviorState AShip::GetBehaviorState() const {
	return behaviorState;
}

bool AShip::InitObject(const int npcID) {
	if (isInited == true || npcID < 0)
		return false;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!USafeENGINE::IsValid(_tempInstance))
		return false;

	FNPCData _tempNpcShipData = _tempInstance->GetNPCData(npcID);
	FShipData _tempShipData = _tempInstance->GetShipData(_tempNpcShipData.ShipID);
	FItemData _tempModuleData;

	npcShipID = _tempNpcShipData.NPCID;
	objectName = _tempNpcShipData.Name;

	UStaticMesh* _newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempShipData.MeshPath.ToString()));
	if (_newMesh)
		objectMesh->SetStaticMesh(_newMesh);

	shipClass = _tempShipData.Shipclass;
	faction = _tempNpcShipData.FactionOfProduction;
	behaviorType = _tempNpcShipData.BehaviorTypeOfNPC;
	lengthRadarRange = FMath::Clamp(_tempShipData.LengthRadarRange, 10.0f, 100000.0f);

	int _tempModuleSlotNumber = FMath::Clamp(FMath::Min3(_tempShipData.SlotTarget, _tempNpcShipData.EquipedSlotTarget.Num(), _tempShipData.HardPoints.Num()), _define_StatModuleSlotMIN, _define_StatModuleSlotMAX);
	slotTargetModule.SetNum(FMath::Clamp(_tempModuleSlotNumber, _define_StatModuleSlotMIN, _define_StatModuleSlotMAX));
	targetingObject.SetNum(FMath::Clamp(_tempModuleSlotNumber, _define_StatModuleSlotMIN, _define_StatModuleSlotMAX));
	for (int index = 0; index < _tempModuleSlotNumber; index++)
		slotTargetModule[index].hardPoint = _tempShipData.HardPoints[index];

	for (int index = 0; index < _tempModuleSlotNumber; index++) {
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
			slotTargetModule[index].ammoCapacity = _tempModuleData.AmmoCapacity;
			slotTargetModule[index].compatibleAmmo = _tempModuleData.UsageAmmo;
			slotTargetModule[index].ammoLifeSpanBonus = _tempModuleData.AmmoLifeSpanBonus;
			if (_tempModuleData.UsageAmmo.Num() > 0)
				slotTargetModule[index].ammo = FItem(_tempModuleData.UsageAmmo[FMath::RandRange(0, _tempModuleData.UsageAmmo.Num() - 1)], FMath::RandRange(0, _tempModuleData.AmmoCapacity));
		}
	}
	lengthToLongAsix = FMath::Clamp(_tempShipData.LengthToLongAsix, _define_StatLengthMIN, _define_StatLengthMAX);
	lengthRadarRange = FMath::Clamp(_tempShipData.LengthRadarRange, _define_StatRadarRangeMIN, _define_StatRadarRangeMAX);
	strategicPoint = FMath::Clamp(_tempShipData.StrategicPoint + _tempNpcShipData.StrategicPointBonus, _define_StatStrategicPointMIN, _define_StatStrategicPointMAX);
	bounty = FMath::Clamp(_tempNpcShipData.NpcBounty, _define_StatBountyMIN, _define_StatBountyMAX);

	maxShield = FMath::Clamp(_tempShipData.Shield, _define_StatDamperMIN, _define_StatDamperMAX);
	rechargeShield = FMath::Clamp(_tempShipData.RechargeShield, _define_StatRestoreMIN, _define_StatRestoreMAX);
	defShield = FMath::Clamp(_tempShipData.DefShield, _define_StatDefMIN, _define_StatDefMAX);
	currentShield = maxShield;

	maxArmor = FMath::Clamp(_tempShipData.Armor, _define_StatDamperMIN, _define_StatDamperMAX);
	repairArmor = FMath::Clamp(_tempShipData.RepairArmor, _define_StatRestoreMIN, _define_StatRestoreMAX);
	defArmor = FMath::Clamp(_tempShipData.DefArmor, _define_StatDefMIN, _define_StatDefMAX);
	currentArmor = maxArmor;

	maxHull = FMath::Clamp(_tempShipData.Hull, _define_StatDamperMIN, _define_StatDamperMAX);
	repairHull = FMath::Clamp(_tempShipData.RepairHull, _define_StatRestoreMIN, _define_StatRestoreMAX);
	defHull = FMath::Clamp(_tempShipData.DefHull, _define_StatDefMIN, _define_StatDefMAX);
	currentHull = maxHull;

	maxPower = FMath::Clamp(_tempShipData.Power, _define_StatDamperMIN, _define_StatDamperMAX);
	rechargePower = FMath::Clamp(_tempShipData.RechargePower, _define_StatRestoreMIN, _define_StatRestoreMAX);
	currentPower = maxPower;

	maxSpeed = FMath::Clamp(_tempShipData.MaxSpeed, _define_StatAccelMIN, _define_StatAccelMAX);
	minAcceleration = FMath::Clamp(_tempShipData.MinAcceleration, _define_StatAccelMIN, _define_StatAccelMAX);
	maxAcceleration = FMath::Clamp(_tempShipData.MaxAcceleration, _define_StatAccelMIN, _define_StatAccelMAX);
	startAccelAngle = FMath::Clamp(_tempShipData.StartAccelAngle, _define_StatRotateMIN, _define_StatRotateMAX);

	maxRotateRate = FMath::Clamp(_tempShipData.MaxRotateRate, _define_StatRotateMIN, _define_StatRotateMAX);
	rotateAcceleration = FMath::Clamp(_tempShipData.RotateAcceleraion, _define_StatRotateMIN, _define_StatRotateMAX);
	rotateDeceleration = FMath::Clamp(_tempShipData.RotateDeceleraion, _define_StatRotateMIN, _define_StatRotateMAX);

	TArray<FBonusStat> _bonusStatsForWork = _tempShipData.bonusStats;
	_bonusStatsForWork.Append(_tempNpcShipData.BonusStats);
	//보너스 스텟의 배열에서 동일 보너스 타입의 엘리먼트들을 1개로 통합하고 나머지는 제거
	for (int index = 0; index < _bonusStatsForWork.Num(); index++) {
		for (int subIndex = 0; subIndex < _bonusStatsForWork.Num(); subIndex++)
			if (_bonusStatsForWork[index].bonusStatType == _bonusStatsForWork[subIndex].bonusStatType && index != subIndex) {
				_bonusStatsForWork[FMath::Min(index, subIndex)].bonusStat = _bonusStatsForWork[FMath::Max(index, subIndex)].bonusStat;
				_bonusStatsForWork.RemoveAtSwap(FMath::Max(index, subIndex));
			}
	}
	_bonusStatsForWork.Shrink();
	for (FBonusStat& tBonusStat : _bonusStatsForWork) {
		switch (tBonusStat.bonusStatType) {
		case BonusStatType::BonusMaxShield:
			maxShield = FMath::Clamp(maxShield * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRechargeShield:
			rechargeShield = FMath::Clamp(rechargeShield * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusDefShield:
			defShield = FMath::Clamp(defShield * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDefMIN, _define_StatDefMAX);
			break;
		case BonusStatType::BonusMaxArmor:
			maxArmor = FMath::Clamp(maxArmor * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRepaireArmor:
			repairArmor = FMath::Clamp(repairArmor * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusDefArmor:
			defArmor = FMath::Clamp(defArmor * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDefMIN, _define_StatDefMAX);
			break;
		case BonusStatType::BonusMaxHull:
			maxHull = FMath::Clamp(maxHull * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRepaireHull:
			repairHull = FMath::Clamp(repairHull * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusDefHull:
			defHull = FMath::Clamp(defHull * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDefMIN, _define_StatDefMAX);
			break;
		case BonusStatType::BonusMaxPower:
			maxPower = FMath::Clamp(maxPower * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRechargePower:
			rechargePower = FMath::Clamp(rechargePower * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusMobilitySpeed:
			maxSpeed = FMath::Clamp(maxSpeed * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatSpeedMIN, _define_StatSpeedMAX);
			break;
		case BonusStatType::BonusMobilityAcceleration:
			maxAcceleration = FMath::Clamp(maxAcceleration * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatAccelMIN, _define_StatAccelMAX);
			minAcceleration = FMath::Clamp(minAcceleration * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatAccelMIN, _define_StatAccelMAX);
			break;
		case BonusStatType::BonusMobilityRotation:
			maxRotateRate = FMath::Clamp(maxRotateRate * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRotateMIN, _define_StatRotateMAX);
			break;
		case BonusStatType::BonusMobilityRotateAcceleration:
			rotateAcceleration = FMath::Clamp(rotateAcceleration * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRotateMIN, _define_StatRotateMAX);
			rotateDeceleration = FMath::Clamp(rotateDeceleration * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRotateMIN, _define_StatRotateMAX);
			break;
		case BonusStatType::BonusMaxRadarRange:
			lengthRadarRange = FMath::Clamp(lengthRadarRange * (1.0f + FMath::Clamp(tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRadarRangeMIN, _define_StatRadarRangeMAX);
			break;
		case BonusStatType::BonusDroneBaseStats:
			bonusDroneBaseStats = FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusDroneControl:
			bonusDroneControl = FMath::Clamp(tBonusStat.bonusStat, _define_StatDroneControlMIN, _define_StatDroneControlMAX);
			break;
		case BonusStatType::BonusDroneBay:
			bonusDroneBay = FMath::Clamp(tBonusStat.bonusStat, _define_StatDroneBayMIN, _define_StatDroneBayMAX);
			break;
		case BonusStatType::BonusDroneDamage:
			bonusDroneDamage = FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusDroneRange:
			bonusDroneRange = FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusDroneSpeed:
			bonusDroneSpeed = FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusTargetModuleUsagePower:
			for (FTargetModule& module : slotTargetModule)
				module.maxUsagePower *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReducePowerMAX, 1.0f - _define_StatBonusReducePowerMIN);
			break;
		default:
			for (int index = 0; index < slotTargetModule.Num(); index++)
				if (slotTargetModule[index].moduleID > 0) {
					switch (slotTargetModule[index].moduleType) {
					case ModuleType::Beam:
						if (tBonusStat.bonusStatType == BonusStatType::BonusBeamDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusBeamCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusBeamAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusBeamRange)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						slotTargetModule[index].ammoLifeSpanBonus = FMath::Clamp(slotTargetModule[index].ammoLifeSpanBonus, 0.0f, slotTargetModule[index].maxCooltime);
						break;
					case ModuleType::Cannon:
						if (tBonusStat.bonusStatType == BonusStatType::BonusCannonDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::Railgun:
						if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::MissileLauncher:
						if (tBonusStat.bonusStatType == BonusStatType::BonusMissileDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::MinerLaser:
						if (tBonusStat.bonusStatType == BonusStatType::BonusMiningAmount)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMiningCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMiningRange)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					default:
						break;
					}
					if (slotTargetModule[index].moduleType == ModuleType::Beam && lengthWeaponRange < slotTargetModule[index].launchSpeedMultiple)
						lengthWeaponRange = slotTargetModule[index].launchSpeedMultiple;
					else if (_tempModuleData.ModuleType > ModuleType::Beam && slotTargetModule[index].moduleType < ModuleType::MinerLaser) {
						lengthWeaponRange = slotTargetModule[index].launchSpeedMultiple;
						_tempModuleData = _tempInstance->GetItemData(slotTargetModule[index].ammo.itemID);
						lengthWeaponRange *= _tempModuleData.LaunchSpeed;
					}
				}
			break;
		}
	}
	return true;
}

bool AShip::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {
	if (isInited)
		return false;
	currentShield = FMath::Clamp(shield, 0.0f, maxShield);
	currentArmor = FMath::Clamp(armor, 0.0f, maxArmor);
	currentHull = FMath::Clamp(hull, 0.0f, maxHull);
	currentPower = FMath::Clamp(power, 0.0f, maxPower);
	return true;
}

float AShip::GetValue(const GetStatType statType) const {
	float _value;
	
	switch (statType) {
	case GetStatType::halfLength:
		_value = lengthToLongAsix * 0.5f;
		break;
	case GetStatType::raderDistance:
		_value = lengthRadarRange;
		break;
	case GetStatType::engageDistance:
		_value = lengthWeaponRange;
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

	case GetStatType::maxRotateRate:
		_value = maxRotateRate;
		break;
	default:
		_value = 0.0f;
		break;
	}
	return _value;
}

void AShip::GetRepaired(const GetStatType statType, float repairValue) {

	repairValue = FMath::Clamp(repairValue, _define_StatRestoreMIN, _define_StatRestoreMAX);
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
	targetSpeed = 0.0f;
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

		int _tmepRange = FMath::Min(slotTargetModule.Num(), targetingObject.Num());
		for (int index = 0; index < _tmepRange; index++) {
			if (slotTargetModule[index].maxUsagePower * slotTargetModule[index].maxCooltime > currentPower)
				continue;
			slotTargetModule[index].moduleState = ModuleState::Activate;
			targetingObject[index] = target;
		}
		return true;
	}
	else return false;
}

bool AShip::CommandMining(AResource* target) {

	if (CheckCanBehavior() == true) {
		targetObject = target;
		behaviorState = BehaviorState::Mining;
		return true;
	}
	else return false;
}

bool AShip::CommandRepair(ASpaceObject* target) {

	return false;
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

bool AShip::CommandLaunch(const TArray<int>& baySlot) {
	return false;
}
#pragma endregion

#pragma region Functions
BehaviorType AShip::GetBehaviorType() const {
	return behaviorType;
}

ShipClass AShip::GetShipClass() const {
	return shipClass;
}

void AShip::GetDockedStructure(TScriptInterface<IStructureable>& getStructure) const {
	getStructure = targetStructure;
}

bool AShip::MoveDistanceCheck() {
	if (bIsStraightMove) {
		targetVector = moveTargetVector;
		realMoveFactor = targetVector - GetActorLocation();
		//DrawDebugCircle(GetWorld(), targetVector, lengthToLongAsix, 40, FColor::Yellow, false, 0.05f, 0, 1.0f, FVector::ForwardVector, FVector::RightVector);
		//DrawDebugLine(GetWorld(), targetVector, GetActorLocation(), FColor::Yellow, false, 0.1f);
	} else {
		if (targetObject != nullptr)
			moveTargetVector = USafeENGINE::CheckLocationMovetoTarget(this, targetObject, 500.0f);

		remainDistance = FVector::Dist(moveTargetVector, GetActorLocation());

		moveTargetVector -= GetActorLocation();
		moveTargetVector.Normalize();
		moveTargetVector = GetActorLocation() + moveTargetVector * remainDistance;
		//DrawDebugCircle(GetWorld(), moveTargetVector, lengthToLongAsix, 40, FColor::Yellow, false, 0.05f, 0, 1.0f, FVector::ForwardVector, FVector::RightVector);

		currentClosedPathIndex = FMath::Clamp(currentClosedPathIndex, 0, FMath::Max(wayPoint.Num() - 1, 0));
		if (wayPoint.Num() > currentClosedPathIndex)
			targetVector = wayPoint[currentClosedPathIndex];
		else return false;

		for (int index = currentClosedPathIndex; index < wayPoint.Num(); index++) {
			//DrawDebugPoint(GetWorld(), wayPoint[index], 5, FColor::Yellow, false, 0.1f);
			if (wayPoint.Num() > index + 1)
				DrawDebugLine(GetWorld(), wayPoint[index], wayPoint[index + 1], FColor::Yellow, false, 0.1f);
		}
	}
	targetVector.Z = 0.0f;
	nextPointDistance = FVector::Dist(targetVector, GetActorLocation());
	targetRotate = realMoveFactor.Rotation() - GetActorRotation();

	//checks distance and Angle For start Acceleration.
	if (nextPointDistance > (FMath::Pow(currentSpeed, 2) / FMath::Clamp(minAcceleration * 2.0f, 1.0f, 9999.0f) + 5.0f)) {
		if (FMath::Abs(targetRotate.Yaw) < startAccelAngle)
			targetSpeed = maxSpeed;
		else targetSpeed = 0.0f;
	} else targetSpeed = 0.0f;

	//arrive to Destination. use upper of Nyquist Rate for high precision.
	if (!bIsStraightMove && nextPointDistance <= FMath::Max(5.0f, currentSpeed * tempDeltaTime * 20.0f)) 
		currentClosedPathIndex = FMath::Clamp(currentClosedPathIndex + 1, 0, wayPoint.Num() - 1);
	//if (remainDistance < currentSpeed * tempDeltaTime * 50.0f) {
	if (remainDistance < lengthToLongAsix * 0.25f) {
		targetSpeed = 0.0f;
		currentClosedPathIndex = 0;
		bIsStraightMove = false;
		return true;
	}
	//arrive to Destination not yet.
	return false;
}

void AShip::RotateCheck() {
	if (bIsStraightMove) {
		targetVector = moveTargetVector;
	} else {
		currentClosedPathIndex = FMath::Clamp(currentClosedPathIndex, 0, wayPoint.Num() - 1);
		if (wayPoint.IsValidIndex(currentClosedPathIndex))
			targetVector = wayPoint[currentClosedPathIndex];
		else return;
	}
	targetVector.Z = 0;

	realMoveFactor = targetVector - GetActorLocation();
	targetRotate = realMoveFactor.Rotation() - GetActorRotation();
	nextPointOuter = FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(GetActorForwardVector(), realMoveFactor));

	if (nextPointOuter > 0.01f) {
		if (FMath::Abs(targetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(rotateDeceleration * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = maxRotateRate;
		else
			targetRotateRateFactor = 0.0f;
	} else if (nextPointOuter < -0.01f) {
		if (FMath::Abs(targetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(rotateDeceleration * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = -maxRotateRate;
		else
			targetRotateRateFactor = 0.0f;
	} else targetRotateRateFactor = 0.0f;
}

void AShip::Movement() {

	if (targetRotateRateFactor > 0.0f) {
		if (realRotateRateFactor >= 0.0f) {
			if (targetRotateRateFactor > realRotateRateFactor)
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateAcceleration * tempDeltaTime, 0.0f, targetRotateRateFactor);
			else
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateDeceleration * tempDeltaTime, targetRotateRateFactor, maxRotateRate);
		} else
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateDeceleration * tempDeltaTime, -maxRotateRate, 0.0f);
	} else if (targetRotateRateFactor < 0.0f) {
		if (realRotateRateFactor <= 0.0f) {
			if (targetRotateRateFactor > realRotateRateFactor)
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateDeceleration * tempDeltaTime, -maxRotateRate, targetRotateRateFactor);
			else
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateAcceleration * tempDeltaTime, targetRotateRateFactor, 0.0f);
		} else
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateDeceleration * tempDeltaTime, 0.0f, maxRotateRate);
	} else {

		if (realRotateRateFactor < 0.0f)
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateDeceleration * tempDeltaTime, -maxRotateRate, 0.0f);
		else
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateDeceleration * tempDeltaTime, 0.0f, maxRotateRate);
	}

	if (currentSpeed < targetSpeed)
		currentSpeed = FMath::Clamp(currentSpeed + maxAcceleration * tempDeltaTime, 0.0f, targetSpeed);
	else if (currentSpeed > targetSpeed)
		currentSpeed = FMath::Clamp(currentSpeed - minAcceleration * tempDeltaTime, 0.0f, maxSpeed);

	AddActorLocalRotation(FRotator(0.0f, realRotateRateFactor, 0.0f) * tempDeltaTime);
	AddActorWorldOffset(GetActorForwardVector() * currentSpeed * tempDeltaTime, true);
}

void AShip::ModuleCheck() {
	moduleConsumptPower = 0.0f;

	FVector _targetedLocation;
	FVector _targetedDirect;
	FRotator _targetedRotation;
	FVector _launchLocation;
	FVector _hardPointLocation;
	FTransform _spawnedTransform;

	for (int index = 0; index < slotTargetModule.Num(); index++) {
		//에너지가 부족할 경우 모든 모듈의 동작을 중지 예약.
		if (currentPower < 5.0f || !targetingObject.IsValidIndex(index))
			slotTargetModule[index].isBookedForOff = true;
		//모듈이 켜져있을 경우 power 소모량 증가 및 쿨타임 지속 감소
		if (slotTargetModule[index].moduleState != ModuleState::NotActivate) {
			slotTargetModule[index].currentUsagePower = FMath::Clamp(slotTargetModule[index].currentUsagePower + slotTargetModule[index].incrementUsagePower * _define_ModuleANDPathTick,
				0.0f, slotTargetModule[index].maxUsagePower);
			slotTargetModule[index].remainCooltime = FMath::Clamp(slotTargetModule[index].remainCooltime - _define_ModuleANDPathTick, 0.0f, FMath::Max(1.0f, slotTargetModule[index].maxCooltime));

			//쿨타임 완료시 행동 실시
			if (slotTargetModule[index].remainCooltime <= 0.0f) {
				switch (slotTargetModule[index].moduleState) {
				case ModuleState::Activate:
				case ModuleState::Overload:
					if (USafeENGINE::IsValid(targetingObject[index]) && targetingObject[index] != this && targetingObject[index]->IsA(ASpaceObject::StaticClass())) {
						//목표 지점 및 발사 위치, 방향 계산
						_targetedLocation = targetingObject[index]->GetActorLocation();
						_targetedDirect = _targetedLocation - GetActorLocation();
						_targetedDirect.Normalize();
						_targetedRotation = _targetedDirect.Rotation();
						_launchLocation = GetActorLocation();

						float _resultDotProduct = FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(GetActorForwardVector(), _targetedDirect));
						if (_resultDotProduct < 0.0f && FMath::Abs(_resultDotProduct) > 0.05f) {
							_launchLocation += GetActorRotation().RotateVector(slotTargetModule[index].hardPoint.leftLaunchPoint);
							_hardPointLocation = slotTargetModule[index].hardPoint.leftLaunchPoint;
						} else if (_resultDotProduct > 0.0f && FMath::Abs(_resultDotProduct) > 0.05) {
							_launchLocation += GetActorRotation().RotateVector(slotTargetModule[index].hardPoint.rightLaunchPoint);
							_hardPointLocation = slotTargetModule[index].hardPoint.rightLaunchPoint;
						} else
							break;

						_spawnedTransform = FTransform(_targetedRotation, _targetedLocation);
						//빔계열 모듈의 경우
						if (slotTargetModule[index].moduleType == ModuleType::Beam ||
							slotTargetModule[index].moduleType == ModuleType::MinerLaser ||
							slotTargetModule[index].moduleType == ModuleType::TractorBeam) {

							ABeam* _beam = Cast<ABeam>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ABeam::StaticClass()
								, _spawnedTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
							if (!USafeENGINE::IsValid(_beam))
								return;
							UGameplayStatics::FinishSpawningActor(_beam, _spawnedTransform);
							_beam->SetBeamProperty(this, targetingObject[index], slotTargetModule[index].launchSpeedMultiple, slotTargetModule[index].moduleType,
								slotTargetModule[index].damageMultiple, _hardPointLocation, slotTargetModule[index].ammoLifeSpanBonus);
						}
						//탄도 무기류의 경우
						else if (slotTargetModule[index].moduleType == ModuleType::Cannon ||
							slotTargetModule[index].moduleType == ModuleType::Railgun ||
							slotTargetModule[index].moduleType == ModuleType::MissileLauncher) {

							if (slotTargetModule[index].ammo.itemAmount < 1) {
								slotTargetModule[index].moduleState = ModuleState::ReloadAmmo;
								slotTargetModule[index].isBookedForOff = false;
							} else {
								slotTargetModule[index].ammo.itemAmount--;

								AProjectiles* _projectile = Cast<AProjectiles>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AProjectiles::StaticClass(),
									_spawnedTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
								if (!USafeENGINE::IsValid(_projectile))
									return;
								UGameplayStatics::FinishSpawningActor(_projectile, _spawnedTransform);
								switch (slotTargetModule[index].moduleType) {
								case ModuleType::Cannon:
								case ModuleType::Railgun:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, this,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, slotTargetModule[index].ammoLifeSpanBonus);
									break;
								case ModuleType::MissileLauncher:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, this,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, slotTargetModule[index].ammoLifeSpanBonus, targetObject);
									break;
								}
							}
						}
					}
					else {
						slotTargetModule[index].moduleState = ModuleState::NotActivate;
						targetObject = nullptr;
						slotTargetModule[index].isBookedForOff = false;
					}
					if (slotTargetModule[index].isBookedForOff) {
						slotTargetModule[index].moduleState = ModuleState::NotActivate;
						targetObject = nullptr;
						slotTargetModule[index].isBookedForOff = false;
					}
					break;
				case ModuleState::ReloadAmmo:
					if (slotTargetModule[index].moduleType == ModuleType::Cannon ||
						slotTargetModule[index].moduleType == ModuleType::Railgun ||
						slotTargetModule[index].moduleType == ModuleType::MissileLauncher) {

						//재장전 실시. 모듈 동작은 재장전이 완료되면 자동으로 시작.
							slotTargetModule[index].ammo.itemAmount = slotTargetModule[index].ammoCapacity;
							if (USafeENGINE::IsValid(targetingObject[index]))
								slotTargetModule[index].moduleState = ModuleState::Activate;
							else 
								slotTargetModule[index].moduleState = ModuleState::NotActivate;
						slotTargetModule[index].isBookedForOff = false;
					}
					break;
				default:
					break;
				}

				slotTargetModule[index].remainCooltime = FMath::Max(1.0f, slotTargetModule[index].maxCooltime);
				//모듈 동작 중지 예약이 활성화되어 있다면 동작 중지.
				if (slotTargetModule[index].isBookedForOff) {
					slotTargetModule[index].moduleState = ModuleState::NotActivate;
					targetObject = nullptr;
					slotTargetModule[index].isBookedForOff = false;
				}
			}
		}
		//모듈이 꺼져있을 경우 power 소모량 감소
		else
			slotTargetModule[index].currentUsagePower = FMath::Clamp(slotTargetModule[index].currentUsagePower - slotTargetModule[index].decrementUsagePower * _define_ModuleANDPathTick,
				0.0f, slotTargetModule[index].maxUsagePower);
		moduleConsumptPower += slotTargetModule[index].currentUsagePower;
	}
}

bool AShip::CheckCanBehavior() const {
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

	bool bMoveTargetHited = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), GetActorLocation() + _forTargetDirectionVector * (lengthToLongAsix * 1.2f)
		, moveTargetVector, FVector(0.0f, lengthToLongAsix * 0.5f + 10.0f, 50.0f), _forTargetDirectionVector.Rotation()
		, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, frontTraceResult, true);
	bool bFrontHited = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), GetActorLocation() + GetActorForwardVector() * (lengthToLongAsix * 1.2f)
		, GetActorLocation() + GetActorForwardVector() * (lengthToLongAsix * 2.0f + 200.0f), FVector(0.0f, lengthToLongAsix * 0.5f + 10.0f, 50.0f), GetActorRotation()
		, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, frontTraceResult, true);

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