// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Ship.h"

AShip::AShip() {
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
}

#pragma region Event Calls
void AShip::BeginPlay()
{
	Super::BeginPlay();

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

		currentShield = FMath::Clamp(currentShield + FMath::Clamp(rechargeShield, _define_StatRestoreMIN, _define_StatRestoreMAX) * _define_ModuleANDPathTick, 0.0f, maxShield);
		currentArmor = FMath::Clamp(currentArmor + FMath::Clamp(repairArmor, _define_StatRestoreMIN, _define_StatRestoreMAX) * _define_ModuleANDPathTick, 0.0f, maxArmor);
		currentHull = FMath::Clamp(currentHull + FMath::Clamp(repairHull, _define_StatRestoreMIN, _define_StatRestoreMAX) * _define_ModuleANDPathTick, 0.0f, maxHull);
		currentPower = FMath::Clamp(currentPower + FMath::Clamp(rechargePower - moduleConsumptPower, -_define_StatRestoreMAX, _define_StatRestoreMAX) * _define_ModuleANDPathTick, 0.0f, maxPower);
		ModuleCheck();
	}

	switch (behaviorState) {
	case BehaviorState::Idle:
		break;
	case BehaviorState::Move:
		if (MoveDistanceCheck()) {
			behaviorState = BehaviorState::Idle;
			moveTargetVector = GetActorLocation() + GetActorForwardVector() * _define_SetDistanceToRotateForward;
		}
		break;
	case BehaviorState::Docking:
		if (MoveDistanceCheck()) {
			moveTargetVector = GetActorLocation() + dockingRotation.RotateVector(FVector::ForwardVector) * _define_SetDistanceToRotateForward;
			targetObject = nullptr;
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
	if (IsValid(_spaceState) && _dealingFaction == Faction::Player)
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

		if (IsValid(_userState) && IsValid(_spaceState) && _dealingFaction == Faction::Player) {
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
		ACargoContainer* _cargoContainer;
		USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
		FNPCData _tempNPCData = _tempInstance->GetNPCData(npcShipID);

		for (FNPCDropData& dropData : _tempNPCData.DropItems) {
			if (FMath::Clamp(dropData.dropChance, _define_DropChanceMIN, _define_DropChanceMAX) < FMath::FRandRange(_define_DropChanceMIN, _define_DropChanceMAX))
				continue;

			_cargoContainer = Cast<ACargoContainer>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ACargoContainer::StaticClass(),
				FTransform(this->GetActorRotation(), this->GetActorLocation()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

			if (_cargoContainer) {
				_cargoContainer->InitObject(FMath::RandRange(0, 13));
				_cargoContainer->SetCargo(FItem(dropData.dropItemID, FMath::RandRange(dropData.dropAmountMin, dropData.dropAmountMax)));
				UGameplayStatics::FinishSpawningActor(_cargoContainer, _cargoContainer->GetTransform());
			}
		}
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Ship][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage,
		_isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void AShip::BeginDestroy() {
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
	if (!IsValid(_tempInstance))
		return false;

	FNPCData _tempNpcShipData = _tempInstance->GetNPCData(npcID);
	FShipData _tempShipData = _tempInstance->GetShipData(_tempNpcShipData.ShipID);
	FItemData _tempModuleData;

	npcShipID = _tempNpcShipData.NPCID;
	objectName = _tempNpcShipData.NPCName;
	if (_tempShipData.FlipSprite) {
		objectFlipBook = _tempShipData.FlipSprite;
		objectSprite->SetSprite(objectFlipBook->GetSpriteAtFrame(0));
	}
	shipClass = _tempShipData.Shipclass;
	faction = _tempNpcShipData.FactionOfProduction;
	behaviorType = _tempNpcShipData.BehaviorTypeOfNPC;

	int _tempModuleSlotNumber = FMath::Clamp(FMath::Min3(_tempShipData.SlotTarget, _tempNpcShipData.EquipedSlotTarget.Num(), _tempShipData.HardPoints.Num()), _define_StatModuleSlotMIN, _define_StatModuleSlotMAX);
	slotTargetModule.SetNum(FMath::Clamp(_tempModuleSlotNumber, _define_StatModuleSlotMIN, _define_StatModuleSlotMAX));
	for (int index = 0; index < _tempModuleSlotNumber; index++) {
		slotTargetModule[index].hardPoint = _tempShipData.HardPoints[index];
		slotTargetModule[index].hardPoint.leftLaunchPoint.Z = 0.0f;
		slotTargetModule[index].hardPoint.rightLaunchPoint.Z = 0.0f;
	}
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

	warpMaxSpeed = FMath::Clamp(_tempShipData.MaxWarpSpeed, _define_WarpSpeedMIN, _define_WarpSpeedMAX);
	warpAcceleration = FMath::Clamp(_tempShipData.WarpAcceleration, _define_WarpAccelerationMIN, _define_WarpAccelerationMAX);
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
			for (FTargetModule& module : slotTargetModule)
				if (module.moduleID > 0) {
					switch (module.moduleType) {
					case ModuleType::Beam:
						if (tBonusStat.bonusStatType == BonusStatType::BonusBeamDamage)
							module.damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusBeamCoolTime)
							module.maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusBeamAccuracy)
							module.accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusBeamRange)
							module.launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						module.ammoLifeSpanBonus = FMath::Clamp(module.ammoLifeSpanBonus, 0.0f, module.maxCooltime);
						break;
					case ModuleType::Cannon:
						if (tBonusStat.bonusStatType == BonusStatType::BonusCannonDamage)
							module.damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonCoolTime)
							module.maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonAccuracy)
							module.accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonLifeTime)
							module.ammoLifeSpanBonus *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusCannonLaunchVelocity)
							module.launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::Railgun:
						if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunDamage)
							module.damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunCoolTime)
							module.maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunAccuracy)
							module.accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunLifeTime)
							module.ammoLifeSpanBonus *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusRailGunLaunchVelocity)
							module.launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::MissileLauncher:
						if (tBonusStat.bonusStatType == BonusStatType::BonusMissileDamage)
							module.damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileCoolTime)
							module.maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileAccuracy)
							module.accaucy *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileLifeTime)
							module.ammoLifeSpanBonus *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMissileLaunchVelocity)
							module.launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::MinerLaser:
						if (tBonusStat.bonusStatType == BonusStatType::BonusMiningAmount)
							module.damageMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMiningCoolTime)
							module.maxCooltime *= FMath::Clamp(1.0f - tBonusStat.bonusStat, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (tBonusStat.bonusStatType == BonusStatType::BonusMiningRange)
							module.launchSpeedMultiple *= FMath::Clamp(1.0f + tBonusStat.bonusStat, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					default:
						break;
					}
				}
		}
	}
	for (FTargetModule& module : slotTargetModule) {
		if (module.moduleType == ModuleType::Beam && lengthWeaponRange < module.launchSpeedMultiple)
			lengthWeaponRange = module.launchSpeedMultiple;
		else if (module.moduleType > ModuleType::Beam && module.moduleType < ModuleType::MinerLaser) {
			lengthWeaponRange = module.launchSpeedMultiple;
			_tempModuleData = _tempInstance->GetItemData(module.ammo.itemID);
			lengthWeaponRange *= _tempModuleData.LaunchSpeed;
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

	case GetStatType::warpMaxSpeed:
		_value = warpMaxSpeed;
		break;
	case GetStatType::warpAcceleration:
		_value = warpAcceleration;
		break;
	case GetStatType::warpCurrentSpeed:
		_value = warpCurrentSpeed;
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
}

bool AShip::CommandMoveToPosition(FVector position) {

	if (CheckCanBehavior() == true) {
		
		moveTargetVector = position;
		targetObject = nullptr;
		behaviorState = BehaviorState::Move;
		return true;
	}
	else return false;
}

bool AShip::CommandMoveToTarget(ASpaceObject* target) {

	if (CheckCanBehavior() == true) {
		targetObject = target;
		behaviorState = BehaviorState::Move;
		return true;
	}
	else return false;
}

bool AShip::CommandAttack(ASpaceObject* target) {

	if (CheckCanBehavior() == true) {
		targetObject = target;
		behaviorState = BehaviorState::Battle;

		for (FTargetModule& module : slotTargetModule) {
			if (module.maxUsagePower * module.maxCooltime > currentPower)
				continue;
			module.moduleState = ModuleState::Activate;
			module.target = target;
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

	if (!CheckCanBehavior() || target == nullptr || !target.GetObjectRef()->IsA(AGate::StaticClass()))
		return false;

	if (this->GetDistanceTo(Cast<AActor>(target.GetObjectRef())) < _define_AvailableDistanceToJump) {
		Destroy();
		return true;
	} else 
		return true;
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
		if (target->RequestedDock(faction, shipClass, dockingLocation, dockingRotation)) {
			targetStructure = target;
			targetObject = Cast<ASpaceObject>(target.GetObjectRef());
			moveTargetVector = targetObject->GetActorLocation() + dockingRotation.RotateVector(dockingLocation);
			behaviorState = BehaviorState::Docking;
			return true;
		}
		else return false;
	}
	return false;
}

bool AShip::CommandUndock() {

	if (behaviorState == BehaviorState::Docked) {
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
	if (IsValid(targetObject) && targetObject->GetObjectType() != ObjectType::Station && targetObject->GetObjectType() != ObjectType::Gate)
		moveTargetVector = targetObject->GetActorLocation();

	remainDistance = FVector::Dist(moveTargetVector, GetActorLocation());
	moveTargetVector.Z = 0.0f;

	//checks distance and Angle For start Acceleration.
	if (remainDistance > (FMath::Pow(currentSpeed, 2) / FMath::Clamp(minAcceleration * 2.0f, 1.0f, 9999.0f) + 5.0f)) {
		if (FMath::Abs(moveTargetRotate.Yaw) < startAccelAngle)
			targetSpeed = targetSpeed * maxSpeed;
		else targetSpeed = 0.0f;
	} else {
		targetSpeed = 0.0f;
		return true;
	}
	//arrive to Destination not yet.
	return false;
}

void AShip::RotateCheck() {

	moveTargetVector.Z = 0;
	moveTargetRotate = (moveTargetVector - GetActorLocation()).Rotation() - GetActorRotation();
	RotationDot = FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(GetActorForwardVector(), moveTargetVector - GetActorLocation()));

	if (RotationDot > 0.01f) {
		if (FMath::Abs(moveTargetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(rotateDeceleration * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = maxRotateRate;
		else
			targetRotateRateFactor = 0.0f;
	} else if (RotationDot < -0.01f) {
		if (FMath::Abs(moveTargetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(rotateDeceleration * 2.0f, 1.0f, 9999.0f)))
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
	FRotator _launchRotation;
	FTransform _spawnedTransform;

	for (FTargetModule& module : slotTargetModule) {
		//에너지가 부족할 경우 모든 모듈의 동작을 중지 예약.
		if (currentPower < 5.0f || !IsValid(module.target))
			module.isBookedForOff = true;
		//모듈이 켜져있을 경우 power 소모량 증가 및 쿨타임 지속 감소
		if (module.moduleState != ModuleState::NotActivate) {
			module.currentUsagePower = FMath::Clamp(module.currentUsagePower + module.incrementUsagePower * _define_ModuleANDPathTick,
				0.0f, module.maxUsagePower);
			module.remainCooltime = FMath::Clamp(module.remainCooltime - _define_ModuleANDPathTick, 0.0f, FMath::Max(1.0f, module.maxCooltime));

			//쿨타임 완료시 행동 실시
			if (module.remainCooltime <= 0.0f) {
				switch (module.moduleState) {
				case ModuleState::Activate:
				case ModuleState::Overload:
					if (IsValid(module.target) && module.target != this && module.target->IsA(ASpaceObject::StaticClass())) {
						//목표 지점 및 발사 위치, 방향 계산
						_targetedLocation = module.target->GetActorLocation();
						_targetedDirect = _targetedLocation - GetActorLocation();
						_targetedDirect.Normalize();
						_targetedRotation = _targetedDirect.Rotation();
						_launchLocation = GetActorLocation();

						float _resultDotProduct = FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(GetActorForwardVector(), _targetedDirect));
						if (_resultDotProduct < 0.0f && FMath::Abs(_resultDotProduct) > 0.05f) {
							_launchLocation += GetActorRotation().RotateVector(module.hardPoint.leftLaunchPoint);
							_hardPointLocation = module.hardPoint.leftLaunchPoint;
							_launchRotation = GetActorRotation();
							_launchRotation.Yaw -= 90.0f;
						} else if (_resultDotProduct > 0.0f && FMath::Abs(_resultDotProduct) > 0.05) {
							_launchLocation += GetActorRotation().RotateVector(module.hardPoint.rightLaunchPoint);
							_hardPointLocation = module.hardPoint.rightLaunchPoint;
							_launchRotation = GetActorRotation();
							_launchRotation.Yaw += 90.0f;
						} else
							break;

						_spawnedTransform = FTransform(_targetedRotation, _targetedLocation);
						//빔계열 모듈의 경우
						if (module.moduleType == ModuleType::Beam ||
							module.moduleType == ModuleType::MinerLaser ||
							module.moduleType == ModuleType::TractorBeam) {

							ABeam* _beam = Cast<ABeam>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ABeam::StaticClass()
								, _spawnedTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
							if (!IsValid(_beam))
								return;
							UGameplayStatics::FinishSpawningActor(_beam, _spawnedTransform);
							_beam->SetBeamProperty(this, module.target, module.launchSpeedMultiple, module.moduleType,
								module.damageMultiple, _hardPointLocation, module.ammoLifeSpanBonus);
						}
						//탄도 무기류의 경우
						else if (module.moduleType == ModuleType::Cannon ||
							module.moduleType == ModuleType::Railgun ||
							module.moduleType == ModuleType::MissileLauncher) {

							if (module.ammo.itemAmount < 1) {
								module.moduleState = ModuleState::ReloadAmmo;
								module.isBookedForOff = false;
							} else {
								module.ammo.itemAmount--;

								AProjectiles* _projectile = Cast<AProjectiles>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AProjectiles::StaticClass(),
									_spawnedTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
								if (!IsValid(_projectile))
									return;
								UGameplayStatics::FinishSpawningActor(_projectile, _spawnedTransform);
								switch (module.moduleType) {
								case ModuleType::Cannon:
								case ModuleType::Railgun:
									_projectile->SetProjectileProperty(module.ammo.itemID, this,
										module.damageMultiple, module.launchSpeedMultiple, module.ammoLifeSpanBonus);
									break;
								case ModuleType::MissileLauncher:
									_projectile->SetActorRotation(_launchRotation);
									_projectile->SetProjectileProperty(module.ammo.itemID, this,
										module.damageMultiple, module.launchSpeedMultiple, module.ammoLifeSpanBonus, targetObject);
									break;
								}
							}
						}
					}
					else {
						module.moduleState = ModuleState::NotActivate;
						targetObject = nullptr;
						module.isBookedForOff = false;
					}
					if (module.isBookedForOff) {
						module.moduleState = ModuleState::NotActivate;
						targetObject = nullptr;
						module.isBookedForOff = false;
					}
					break;
				case ModuleState::ReloadAmmo:
					if (module.moduleType == ModuleType::Cannon ||
						module.moduleType == ModuleType::Railgun ||
						module.moduleType == ModuleType::MissileLauncher) {

						//재장전 실시. 모듈 동작은 재장전이 완료되면 자동으로 시작.
							module.ammo.itemAmount = module.ammoCapacity;
							if (IsValid(module.target))
								module.moduleState = ModuleState::Activate;
							else 
								module.moduleState = ModuleState::NotActivate;
						module.isBookedForOff = false;
					}
					break;
				default:
					break;
				}

				module.remainCooltime = FMath::Max(1.0f, module.maxCooltime);
				//모듈 동작 중지 예약이 활성화되어 있다면 동작 중지.
				if (module.isBookedForOff) {
					module.moduleState = ModuleState::NotActivate;
					targetObject = nullptr;
					module.isBookedForOff = false;
				}
			}
		}
		//모듈이 꺼져있을 경우 power 소모량 감소
		else
			module.currentUsagePower = FMath::Clamp(module.currentUsagePower - module.decrementUsagePower * _define_ModuleANDPathTick,
				0.0f, module.maxUsagePower);
		moduleConsumptPower += module.currentUsagePower;
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
