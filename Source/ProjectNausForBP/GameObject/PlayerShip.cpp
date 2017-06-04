// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "PlayerShip.h"

APlayerShip::APlayerShip() {
	objectMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("ObjectMovement"));
	objectMovement->SetPlaneConstraintEnabled(true);
	objectMovement->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);

	playerViewpointCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FixedCamera"));
	playerViewpointCamera->SetupAttachment(RootComponent, RootComponent->GetAttachSocketName());
	playerViewpointCamera->bAbsoluteRotation = true;
	playerViewpointCamera->AddWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	playerViewpointCamera->SetProjectionMode(ECameraProjectionMode::Orthographic);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0f;

	sShipID = -1;
	checkTime = 0.0f;
	sIsInited = false;

	slotTargetModule = TArray<FTargetModule>();
	slotActiveModule = TArray<FActiveModule>();
	slotPassiveModule = TArray<int>();
	slotSystemModule = TArray<int>();
}

#pragma region Event Calls
void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	if (Cast<APawn>(this) != UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][Begin] Spawn Fail! this is not User's Ship!"));
		Destroy();
	}

	sShipID = -1;
	checkTime = 0.0f;
	sIsInited = false;
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][Begin] Spawn Finish!"));
}

void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (FTargetModule& module : slotTargetModule) {
		DrawDebugPoint(GetWorld(), GetActorLocation() + GetActorRotation().RotateVector(module.hardPoint.leftLaunchPoint), 5.0f, FColor::Blue, false, 0.1f);
		DrawDebugPoint(GetWorld(), GetActorLocation() + GetActorRotation().RotateVector(module.hardPoint.rightLaunchPoint), 5.0f, FColor::Red, false, 0.1f);
	}

	tempDeltaTime = DeltaTime;
	checkTime += DeltaTime;
	if (checkTime >_define_ModuleANDPathTick) {
		ModuleCheck();
		checkTime = 0.0f;
	}
	sCurrentHull = FMath::Clamp(sCurrentHull + FMath::Clamp(sRepairHull + moduleStatHullRepair, _define_StatRestoreMIN, _define_StatRestoreMAX) * DeltaTime, 0.0f, sMaxHull);
	sCurrentArmor = FMath::Clamp(sCurrentArmor + FMath::Clamp(sRepairArmor + moduleStatArmorRepair, _define_StatRestoreMIN, _define_StatRestoreMAX) * DeltaTime, 0.0f, sMaxArmor);
	sCurrentShield = FMath::Clamp(sCurrentShield + FMath::Clamp(sRechargeShield + moduleStatShieldRegen, _define_StatRestoreMIN, _define_StatRestoreMAX) * DeltaTime, 0.0f, sMaxShield);
	sCurrentPower = FMath::Clamp(sCurrentPower + FMath::Clamp(sRechargePower - moduleConsumptPower, -_define_StatRestoreMAX, _define_StatRestoreMAX) * DeltaTime, 0.0f, sMaxPower);

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
	default:
		break;
	}
	RotateCheck();
	Movement();
}

float APlayerShip::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
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
	if (sCurrentShield > _remainDamage) {
		_effectReduceDamage = FMath::Pow((FMath::Clamp(sDefShield, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
		_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
		_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
		_effectShieldDamage = _remainDamage;
		sCurrentShield -= _remainDamage;
		_remainDamage = 0.0f;
	} else {
		_effectShieldDamage = sCurrentShield;
		_remainDamage -= _effectShieldDamage;
		sCurrentShield = 0.0f;

		if (sCurrentArmor > _remainDamage) {
			_effectReduceDamage = FMath::Pow((FMath::Clamp(sDefArmor, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
			_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
			_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
			_effectArmorDamage = _remainDamage;
			sCurrentArmor -= _remainDamage;
			_remainDamage = 0.0f;
		} else {
			_effectArmorDamage = sCurrentArmor;
			_remainDamage -= _effectArmorDamage;
			sCurrentArmor = 0.0f;

			_effectReduceDamage = FMath::Pow((FMath::Clamp(sDefHull, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
			_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
			_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
			_effectHullDamage = _remainDamage;
			sCurrentHull -= _remainDamage;
			_remainDamage = 0.0f;
		}
	}

	if (sCurrentHull <= 0.0f) {
		_effectHullDamage = sCurrentHull;
		sCurrentHull = 0.0f;
		AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
		Peer _peerResult = Peer::Neutral;

		if (IsValid(_userState)) {
			_userState->ChangeBounty(-_userState->GetBounty());
			_peerResult = _spaceState->PeerIdentify(faction, _dealingFaction, true);
			//전략 포인트의 일부 가중치를 팩션 관계도에 반영
			_spaceState->ApplyRelation(_dealingFaction, strategicPoint, true);
			//사망 처리
			_userState->PlayerDeath();
		}
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Ship][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage,
		_isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void APlayerShip::SetupPlayerInputComponent(class UInputComponent* inputComponent)
{
	Super::SetupPlayerInputComponent(inputComponent);
}

void APlayerShip::BeginDestroy() {
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int APlayerShip::GetObjectID() const {
	return sShipID;
}

ObjectType APlayerShip::GetObjectType() const {
	return ObjectType::Ship;
}

Faction APlayerShip::GetFaction() const {
	return Faction::Player;
}

void APlayerShip::SetFaction(const Faction setFaction) {
	return;
}

BehaviorState APlayerShip::GetBehaviorState() const {
	return behaviorState;
}

bool APlayerShip::InitObject(const int objectId) {
	if (sIsInited == true && (objectId < 0 || objectId == sShipID))
		return false;

	UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][InitObject] before init ID : %d, Init to ID : %d"), sShipID, objectId);
	sShipID = objectId;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!IsValid(_tempInstance))
		return false;
	FShipData _tempShipData = _tempInstance->GetShipData(objectId);

	objectName = _tempShipData.ShipName;
	if (_tempShipData.FlipSprite) {
		objectFlipBook = _tempShipData.FlipSprite;
		objectSprite->SetSprite(objectFlipBook->GetSpriteAtFrame(0));
	}

	int _tempModuleSlotNumber = FMath::Clamp(FMath::Min(_tempShipData.SlotTarget, _tempShipData.HardPoints.Num()), _define_StatModuleSlotMIN, _define_StatModuleSlotMAX);
	slotTargetModule.SetNum(FMath::Clamp(_tempModuleSlotNumber, _define_StatModuleSlotMIN, _define_StatModuleSlotMAX));
	for (int index = 0; index < _tempModuleSlotNumber; index++) {
		slotTargetModule[index].hardPoint = _tempShipData.HardPoints[index];
		slotTargetModule[index].hardPoint.leftLaunchPoint.Z = 0.0f;
		slotTargetModule[index].hardPoint.rightLaunchPoint.Z = 0.0f;
	}
	slotActiveModule.SetNum(FMath::Clamp(_tempShipData.SlotActive, _define_StatModuleSlotMIN, _define_StatModuleSlotMAX));
	slotPassiveModule.SetNum(FMath::Clamp(_tempShipData.SlotPassive, _define_StatModuleSlotMIN, _define_StatModuleSlotMAX));
	slotSystemModule.SetNum(FMath::Clamp(_tempShipData.SlotSystem, _define_StatModuleSlotMIN, _define_StatModuleSlotMAX));

	UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][InitObject] Init Module Slots : %d, %d, %d, %d"), slotTargetModule.Num(), slotActiveModule.Num(), slotPassiveModule.Num(), slotSystemModule.Num());
	sMaxShield = FMath::Clamp(_tempShipData.Shield, _define_StatDamperMIN, _define_StatDamperMAX);
	sRechargeShield = FMath::Clamp(_tempShipData.RechargeShield, _define_StatRestoreMIN, _define_StatRestoreMAX);
	sDefShield = FMath::Clamp(_tempShipData.DefShield, _define_StatDefMIN, _define_StatDefMAX);

	sMaxArmor = FMath::Clamp(_tempShipData.Armor, _define_StatDamperMIN, _define_StatDamperMAX);
	sRepairArmor = FMath::Clamp(_tempShipData.RepairArmor, _define_StatRestoreMIN, _define_StatRestoreMAX);
	sDefArmor = FMath::Clamp(_tempShipData.DefArmor, _define_StatDefMIN, _define_StatDefMAX);

	sMaxHull = FMath::Clamp(_tempShipData.Hull, _define_StatDamperMIN, _define_StatDamperMAX);
	sRepairHull = FMath::Clamp(_tempShipData.RepairHull, _define_StatRestoreMIN, _define_StatRestoreMAX);
	sDefHull = FMath::Clamp(_tempShipData.DefHull, _define_StatDefMIN, _define_StatDefMAX);

	sMaxPower = FMath::Clamp(_tempShipData.Power, _define_StatDamperMIN, _define_StatDamperMAX);
	sRechargePower = FMath::Clamp(_tempShipData.RechargePower, _define_StatRestoreMIN, _define_StatRestoreMAX);

	sMaxCompute = FMath::Clamp(_tempShipData.Compute, _define_StatComputePerformanceMIN, _define_StatComputePerformanceMAX);
	sMaxPowerGrid = FMath::Clamp(_tempShipData.PowerGrid, _define_StatPowerGridPerformanceMIN, _define_StatPowerGridPerformanceMAX);
	sMaxCargo = FMath::Clamp(_tempShipData.Cargo, _define_StatCargoSizeMIN, _define_StatCargoSizeMAX);
	lengthRadarRange = FMath::Clamp(_tempShipData.LengthRadarRange, _define_StatRadarRangeMIN, _define_StatRadarRangeMAX);
	strategicPoint = FMath::Clamp(_tempShipData.StrategicPoint, _define_StatStrategicPointMIN, _define_StatStrategicPointMAX);

	sWarpMaxSpeed = FMath::Clamp(_tempShipData.MaxWarpSpeed, _define_WarpSpeedMIN, _define_WarpSpeedMAX);
	sWarpAcceleration = FMath::Clamp(_tempShipData.WarpAcceleration, _define_WarpAccelerationMIN, _define_WarpAccelerationMAX);
	sMaxSpeed = FMath::Clamp(_tempShipData.MaxSpeed, _define_StatAccelMIN, _define_StatAccelMAX);
	sMinAcceleration = FMath::Clamp(_tempShipData.MinAcceleration, _define_StatAccelMIN, _define_StatAccelMAX);
	sMaxAcceleration = FMath::Clamp(_tempShipData.MaxAcceleration, _define_StatAccelMIN, _define_StatAccelMAX);
	sStartAccelAngle = FMath::Clamp(_tempShipData.StartAccelAngle, _define_StatRotateMIN, _define_StatRotateMAX);

	sMaxRotateRate = FMath::Clamp(_tempShipData.MaxRotateRate, _define_StatRotateMIN, _define_StatRotateMAX);
	sRotateAcceleration = FMath::Clamp(_tempShipData.RotateAcceleraion, _define_StatRotateMIN, _define_StatRotateMAX);
	sRotateDeceleration = FMath::Clamp(_tempShipData.RotateDeceleraion, _define_StatRotateMIN, _define_StatRotateMAX);

	if (TotalStatsUpdate() == false)
		return false;
	return true;
}

bool APlayerShip::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {
	return false;
}

float APlayerShip::GetValue(const GetStatType statType) const {
	float _value;

	switch (statType) {
	case GetStatType::raderDistance:
		_value = lengthRadarRange;
		break;
	case GetStatType::engageDistance:
		_value = lengthWeaponRange;
		break;
	case GetStatType::maxShield:
		_value = sMaxShield;
		break;
	case GetStatType::rechargeShield:
		_value = sRechargeShield;
		break;
	case GetStatType::currentShield:
		_value = sCurrentShield;
		break;
	case GetStatType::defShield:
		_value = sDefShield;
		break;

	case GetStatType::maxArmor:
		_value = sMaxArmor;
		break;
	case GetStatType::repaireArmor:
		_value = sRepairArmor;
		break;
	case GetStatType::currentArmor:
		_value = sCurrentArmor;
		break;
	case GetStatType::defArmor:
		_value = sDefArmor;
		break;

	case GetStatType::maxHull:
		_value = sMaxHull;
		break;
	case GetStatType::repaireHull:
		_value = sRepairHull;
		break;
	case GetStatType::currentHull:
		_value = sCurrentHull;
		break;
	case GetStatType::defHull:
		_value = sDefHull;
		break;

	case GetStatType::maxPower:
		_value = sMaxPower;
		break;
	case GetStatType::rechargePower:
		_value = sRechargePower;
		break;
	case GetStatType::currentPower:
		_value = sCurrentPower;
		break;

	case GetStatType::maxCompute:
		_value = sMaxCompute;
		break;
	case GetStatType::currentCompute:
		_value = CalculateCompute();
		break;
	case GetStatType::maxPowerGrid:
		_value = sMaxPowerGrid;
		break;
	case GetStatType::currentPowerGrid:
		_value = CalculatePowerGrid();
		break;
	case GetStatType::maxCargo:
		_value = sMaxCargo;
		break;

	case GetStatType::warpMaxSpeed:
		_value = sWarpMaxSpeed;
		break;
	case GetStatType::warpAcceleration:
		_value = sWarpAcceleration;
		break;
	case GetStatType::warpCurrentSpeed:
		_value = sWarpCurrentSpeed;
		break;
	case GetStatType::maxSpeed:
		_value = sMaxSpeed;
		break;
	case GetStatType::currentSpeed:
		_value = currentSpeed;
		break;

	case GetStatType::maxAcceleration:
		_value = sMaxAcceleration;
		break;
	case GetStatType::minAcceleration:
		_value = sMinAcceleration;
		break;
	case GetStatType::currentAcceleration:
		_value = accelerationFactor;
		break;

	case GetStatType::maxRotateRate:
		_value = sMaxRotateRate * (1.0f + moduleStatThruster);
		break;
	case GetStatType::currentRotateRate:
		_value = rotateRateFactor;
		break;
	case GetStatType::rotateAcceleraion:
		_value = sRotateAcceleration;
		break;
	case GetStatType::rotateDeceleraion:
		_value = sRotateDeceleration;
		break;

	case GetStatType::droneBaseStatsMultiple:
		_value = bonusDroneBaseStats;
		break;
	case GetStatType::droneDroneDamageMultiple:
		_value = bonusDroneDamage;
		break;
	case GetStatType::droneDroneRangeMultiple:
		_value = bonusDroneRange;
		break;
	case GetStatType::droneDroneSpeedMultiple:
		_value = bonusDroneSpeed;
		break;
	case GetStatType::droneDroneControlMultiple:
		_value = bonusDroneControl;
		break;
	default:
		_value = 0.0f;
		break;
	}
	return _value;
}

void APlayerShip::GetRepaired(const GetStatType statType, float repairValue) {

	repairValue = FMath::Clamp(repairValue, _define_StatRestoreMIN, _define_StatRestoreMAX);
	switch (statType) {
	case GetStatType::currentShield:
		sCurrentShield = FMath::Clamp(sCurrentShield + repairValue, 0.0f, sMaxShield);
		break;
	case GetStatType::currentArmor:
		sCurrentArmor = FMath::Clamp(sCurrentArmor + repairValue, 0.0f, sMaxArmor);
		break;
	case GetStatType::currentHull:
		sCurrentHull = FMath::Clamp(sCurrentHull + repairValue, 0.0f, sMaxHull);
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region Funcations : PlayerShip Only
bool APlayerShip::TotalStatsUpdate() {

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	AUserState* _tempUserState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
	if (!IsValid(_tempInstance) || !IsValid(_tempUserState))
		return false;
	FShipData _tempShipData = _tempInstance->GetShipData(sShipID);
	
	FItemData _tempModuleData;
	TArray<FSkillIDANDLevel> _skillArray;
	FSkillData _tempSkillData;
	TMap<BonusStatType, float> _tempbonusStats;
	bool isFindSameAttribute = false;

	sMaxShield = FMath::Clamp(_tempShipData.Shield, _define_StatDamperMIN, _define_StatDamperMAX);
	sRechargeShield = FMath::Clamp(_tempShipData.RechargeShield, _define_StatRestoreMIN, _define_StatRestoreMAX);
	sDefShield = FMath::Clamp(_tempShipData.DefShield, _define_StatDefMIN, _define_StatDefMAX);

	sMaxArmor = FMath::Clamp(_tempShipData.Armor, _define_StatDamperMIN, _define_StatDamperMAX);
	sRepairArmor = FMath::Clamp(_tempShipData.RepairArmor, _define_StatRestoreMIN, _define_StatRestoreMAX);
	sDefArmor = FMath::Clamp(_tempShipData.DefArmor, _define_StatDefMIN, _define_StatDefMAX);

	sMaxHull = FMath::Clamp(_tempShipData.Hull, _define_StatDamperMIN, _define_StatDamperMAX);
	sRepairHull = FMath::Clamp(_tempShipData.RepairHull, _define_StatRestoreMIN, _define_StatRestoreMAX);
	sDefHull = FMath::Clamp(_tempShipData.DefHull, _define_StatDefMIN, _define_StatDefMAX);

	sMaxPower = FMath::Clamp(_tempShipData.Power, _define_StatDamperMIN, _define_StatDamperMAX);
	sRechargePower = FMath::Clamp(_tempShipData.RechargePower, _define_StatRestoreMIN, _define_StatRestoreMAX);

	sMaxCompute = FMath::Clamp(_tempShipData.Compute, _define_StatComputePerformanceMIN, _define_StatComputePerformanceMAX);
	sMaxPowerGrid = FMath::Clamp(_tempShipData.PowerGrid, _define_StatPowerGridPerformanceMIN, _define_StatPowerGridPerformanceMAX);
	sMaxCargo = FMath::Clamp(_tempShipData.Cargo, _define_StatCargoSizeMIN, _define_StatCargoSizeMAX);
	lengthRadarRange = FMath::Clamp(_tempShipData.LengthRadarRange, _define_StatRadarRangeMIN, _define_StatRadarRangeMAX);

	sWarpMaxSpeed = FMath::Clamp(_tempShipData.MaxWarpSpeed, _define_WarpSpeedMIN, _define_WarpSpeedMAX);
	sWarpAcceleration = FMath::Clamp(_tempShipData.WarpAcceleration, _define_WarpAccelerationMIN, _define_WarpAccelerationMAX);
	sMaxSpeed = FMath::Clamp(_tempShipData.MaxSpeed, _define_StatAccelMIN, _define_StatAccelMAX);
	sMinAcceleration = FMath::Clamp(_tempShipData.MinAcceleration, _define_StatAccelMIN, _define_StatAccelMAX);
	sMaxAcceleration = FMath::Clamp(_tempShipData.MaxAcceleration, _define_StatAccelMIN, _define_StatAccelMAX);
	sStartAccelAngle = FMath::Clamp(_tempShipData.StartAccelAngle, _define_StatRotateMIN, _define_StatRotateMAX);

	sMaxRotateRate = FMath::Clamp(_tempShipData.MaxRotateRate, _define_StatRotateMIN, _define_StatRotateMAX);
	sRotateAcceleration = FMath::Clamp(_tempShipData.RotateAcceleraion, _define_StatRotateMIN, _define_StatRotateMAX);
	sRotateDeceleration = FMath::Clamp(_tempShipData.RotateDeceleraion, _define_StatRotateMIN, _define_StatRotateMAX);

	for (int index = 0; index < slotPassiveModule.Num(); index++) {
		if (slotPassiveModule[index] > 0) {
			_tempModuleData = _tempInstance->GetItemData(slotPassiveModule[index]);
			CheckPassiveTypeModule(_tempModuleData.StatType, _tempModuleData.BonusStatValue);
		}
	}
	for (int index = 0; index < slotSystemModule.Num(); index++) {
		if (slotSystemModule[index] > 0) {
			_tempModuleData = _tempInstance->GetItemData(slotSystemModule[index]);
			CheckPassiveTypeModule(_tempModuleData.StatType, _tempModuleData.BonusStatValue);
		}
	}

	Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->GetUserDataSkill(_skillArray);
	_tempbonusStats = _tempShipData.bonusStats;
	for (FSkillIDANDLevel& skill : _skillArray) {
		_tempSkillData = _tempInstance->GetSkillData(skill.skillID);
		for (auto& bonusDataInSkill : _tempSkillData.bonusStatPerLevel) {
			if (_tempbonusStats.Contains(bonusDataInSkill.Key))
				_tempbonusStats[bonusDataInSkill.Key] += bonusDataInSkill.Value * skill.skillLevel;
			else _tempbonusStats.Emplace(bonusDataInSkill.Key, bonusDataInSkill.Value * skill.skillLevel);
		}
	}
	for (auto& bonusStat : _tempbonusStats) {
		switch (bonusStat.Key) {
		case BonusStatType::BonusMaxShield:
			sMaxShield = FMath::Clamp(sMaxShield * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRechargeShield:
			sRechargeShield = FMath::Clamp(sRechargeShield * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusDefShield:
			sDefShield = FMath::Clamp(sDefShield * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDefMIN, _define_StatDefMAX);
			break;
		case BonusStatType::BonusMaxArmor:
			sMaxArmor = FMath::Clamp(sMaxArmor * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRepaireArmor:
			sRepairArmor = FMath::Clamp(sRepairArmor * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusDefArmor:
			sDefArmor = FMath::Clamp(sDefArmor * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDefMIN, _define_StatDefMAX);
			break;
		case BonusStatType::BonusMaxHull:
			sMaxHull = FMath::Clamp(sMaxHull * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRepaireHull:
			sRepairHull = FMath::Clamp(sRepairHull * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusDefHull:
			sDefHull = FMath::Clamp(sDefHull * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDefMIN, _define_StatDefMAX);
			break;
		case BonusStatType::BonusMaxPower:
			sMaxPower = FMath::Clamp(sMaxPower * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatDamperMIN, _define_StatDamperMAX);
			break;
		case BonusStatType::BonusRechargePower:
			sRechargePower = FMath::Clamp(sRechargePower * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRestoreMIN, _define_StatRestoreMAX);
			break;
		case BonusStatType::BonusMobilitySpeed:
			sMaxSpeed = FMath::Clamp(sMaxSpeed * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatSpeedMIN, _define_StatSpeedMAX);
			break;
		case BonusStatType::BonusMobilityAcceleration:
			sMaxAcceleration = FMath::Clamp(sMaxAcceleration * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatAccelMIN, _define_StatAccelMAX);
			sMinAcceleration = FMath::Clamp(sMinAcceleration * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatAccelMIN, _define_StatAccelMAX);
			break;
		case BonusStatType::BonusMobilityRotation:
			sMaxRotateRate = FMath::Clamp(sMaxRotateRate * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRotateMIN, _define_StatRotateMAX);
			break;
		case BonusStatType::BonusMobilityRotateAcceleration:
			sRotateAcceleration = FMath::Clamp(sRotateAcceleration * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRotateMIN, _define_StatRotateMAX);
			sRotateDeceleration = FMath::Clamp(sRotateDeceleration * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRotateMIN, _define_StatRotateMAX);
			break;
		case BonusStatType::BonusMaxRadarRange:
			lengthRadarRange = FMath::Clamp(lengthRadarRange * (1.0f + FMath::Clamp(bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX)), _define_StatRadarRangeMIN, _define_StatRadarRangeMAX);
			break;
		case BonusStatType::BonusMaxCPUPerfomance:
			sMaxCompute = FMath::Clamp(sMaxCompute + bonusStat.Value, _define_StatComputePerformanceMIN, _define_StatComputePerformanceMAX);
			break;
		case BonusStatType::BonusMaxPowerGridPerfomance:
			sMaxPowerGrid = FMath::Clamp(sMaxPowerGrid + bonusStat.Value, _define_StatPowerGridPerformanceMIN, _define_StatPowerGridPerformanceMAX);
			break;
		case BonusStatType::BonusMaxCargoSize:
			sMaxCargo = FMath::Clamp(sMaxCargo + bonusStat.Value, _define_StatCargoSizeMIN, _define_StatCargoSizeMAX);
			break;
		case BonusStatType::BonusDroneBaseStats:
			bonusDroneBaseStats = FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusDroneControl:
			bonusDroneControl = FMath::Clamp(bonusStat.Value, _define_StatDroneControlMIN, _define_StatDroneControlMAX);
			break;
		case BonusStatType::BonusDroneBay:
			bonusDroneBay = FMath::Clamp(bonusStat.Value, _define_StatDroneBayMIN, _define_StatDroneBayMAX);
			break;
		case BonusStatType::BonusDroneDamage:
			bonusDroneDamage = FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusDroneRange:
			bonusDroneRange = FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusDroneSpeed:
			bonusDroneSpeed = FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveRechargeShield:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveDefShield:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveRepaireArmor:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveDefArmor:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveRepaireHull:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveDefHull:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveSpeed:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveAcceleration:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveRotation:
			for (FActiveModule& module : slotActiveModule)
				if (module.moduleType == ModuleType::ShieldGenerator)
					module.maxActiveModuleFactor *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
			break;
		case BonusStatType::BonusActiveModuleUsagePower:
			for (FActiveModule& module : slotActiveModule)
				module.maxActiveModuleFactor *= FMath::Clamp(1.0f - bonusStat.Value, 1.0f - _define_StatBonusReducePowerMAX, 1.0f - _define_StatBonusReducePowerMIN);
			break;
		case BonusStatType::BonusTargetModuleUsagePower:
			for (FTargetModule& module : slotTargetModule)
				module.maxUsagePower *= FMath::Clamp(1.0f - bonusStat.Value, 1.0f - _define_StatBonusReducePowerMAX, 1.0f - _define_StatBonusReducePowerMIN);
			break;
		default:
			for (int index = 0; index < slotTargetModule.Num(); index++)
				if (slotTargetModule[index].moduleID > 0) {
					switch (slotTargetModule[index].moduleType) {
					case ModuleType::Beam:
						if (bonusStat.Key == BonusStatType::BonusBeamDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusBeamCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.Value, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (bonusStat.Key == BonusStatType::BonusBeamAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusBeamRange)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						slotTargetModule[index].ammoLifeSpanBonus = FMath::Clamp(slotTargetModule[index].ammoLifeSpanBonus, 0.0f, slotTargetModule[index].maxCooltime);
						break;
					case ModuleType::Cannon:
						if (bonusStat.Key == BonusStatType::BonusCannonDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusCannonCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.Value, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (bonusStat.Key == BonusStatType::BonusCannonAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusCannonLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusCannonLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::Railgun:
						if (bonusStat.Key == BonusStatType::BonusRailGunDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusRailGunCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.Value, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (bonusStat.Key == BonusStatType::BonusRailGunAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusRailGunLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusRailGunLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::MissileLauncher:
						if (bonusStat.Key == BonusStatType::BonusMissileDamage)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusMissileCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.Value, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (bonusStat.Key == BonusStatType::BonusMissileAccuracy)
							slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusMissileLifeTime)
							slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusMissileLaunchVelocity)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					case ModuleType::MinerBeam:
						if (bonusStat.Key == BonusStatType::BonusMiningAmount)
							slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						else if (bonusStat.Key == BonusStatType::BonusMiningCoolTime)
							slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - bonusStat.Value, 1.0f - _define_StatBonusReduceCooltimeMAX, 1.0f - _define_StatBonusReduceCooltimeMAX);
						else if (bonusStat.Key == BonusStatType::BonusMiningRange)
							slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + bonusStat.Value, _define_StatBonusMIN + 1.0f, _define_StatBonusMAX);
						break;
					default:
						break;
					}
				}
			break;
		}
	}
	for (FTargetModule& targetModule : slotTargetModule) {
		if (targetModule.moduleType == ModuleType::Beam && lengthWeaponRange < targetModule.launchSpeedMultiple)
			lengthWeaponRange = targetModule.launchSpeedMultiple;
		else if (_tempModuleData.ModuleType > ModuleType::Beam && targetModule.moduleType < ModuleType::MinerBeam) {
			lengthWeaponRange = targetModule.launchSpeedMultiple;
			_tempModuleData = _tempInstance->GetItemData(targetModule.ammo.itemID);
			lengthWeaponRange *= _tempModuleData.LaunchSpeed;
		}
	}

	Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUI(UpdateUIType::Profile_Ship);
	UE_LOG(LogClass, Log, TEXT("Ship Totaly Init complete!"));
	return true;
}

void APlayerShip::CheckPassiveTypeModule(const BonusStatType type, float value) {
	switch (type) {
	case BonusStatType::BonusMaxShield:					
		sMaxShield = FMath::Clamp(sMaxShield + value, _define_StatDamperMIN, _define_StatDamperMAX);
		break;
	case BonusStatType::BonusRechargeShield:			
		sRechargeShield = FMath::Clamp(sRechargeShield + value, _define_StatRestoreMIN, _define_StatRestoreMAX);
		break;
	case BonusStatType::BonusDefShield:					
		sDefShield = FMath::Clamp(sDefShield + value, _define_StatDefMIN, _define_StatDefMAX);
		break;

	case BonusStatType::BonusMaxArmor:					
		sMaxArmor = FMath::Clamp(sMaxArmor + value, _define_StatDamperMIN, _define_StatDamperMAX);
		break;
	case BonusStatType::BonusRepaireArmor:				
		sRepairArmor = FMath::Clamp(sRepairArmor + value, _define_StatRestoreMIN, _define_StatRestoreMAX);
		break;
	case BonusStatType::BonusDefArmor:					
		sDefArmor = FMath::Clamp(sDefArmor + value, _define_StatDefMIN, _define_StatDefMAX);
		break;

	case BonusStatType::BonusMaxHull:					
		sMaxHull = FMath::Clamp(sMaxHull + value, _define_StatDamperMIN, _define_StatDamperMAX);
		break;
	case BonusStatType::BonusRepaireHull:				
		sRepairHull = FMath::Clamp(sRepairHull + value, _define_StatRestoreMIN, _define_StatRestoreMAX);
		break;
	case BonusStatType::BonusDefHull:					
		sDefHull = FMath::Clamp(sDefHull + value, _define_StatDefMIN, _define_StatDefMAX);
		break;

	case BonusStatType::BonusMaxPower:					
		sMaxPower = FMath::Clamp(sMaxPower + value, _define_StatDamperMIN, _define_StatDamperMAX);
		break;
	case BonusStatType::BonusRechargePower:				
		sRechargePower = FMath::Clamp(sRechargePower + value, _define_StatRestoreMIN, _define_StatRestoreMAX);
		break;

	case BonusStatType::BonusMobilitySpeed:				
		sMaxSpeed = FMath::Clamp(sMaxSpeed * (1.0f + value), _define_StatAccelMIN, _define_StatAccelMAX);
		break;
	case BonusStatType::BonusMobilityAcceleration:
		sMinAcceleration = FMath::Clamp(sMinAcceleration * (1.0f + value), _define_StatAccelMIN, _define_StatAccelMAX);
		sMaxAcceleration = FMath::Clamp(sMaxAcceleration * (1.0f + value), _define_StatAccelMIN, _define_StatAccelMAX);
		break;
	case BonusStatType::BonusMobilityRotation:			
		sMaxRotateRate = FMath::Clamp(sMaxRotateRate * (1.0f + value), _define_StatRotateMIN, _define_StatRotateMAX);
		break;
	case BonusStatType::BonusMobilityRotateAcceleration:
		sRotateAcceleration = FMath::Clamp(sRotateAcceleration * (1.0f + value), _define_StatRotateMIN, _define_StatRotateMAX);
		sRotateDeceleration = FMath::Clamp(sRotateDeceleration * (1.0f + value), _define_StatRotateMIN, _define_StatRotateMAX);
		break;
	case BonusStatType::BonusMaxRadarRange:				
		lengthRadarRange = FMath::Clamp(lengthRadarRange * (1.0f + value), _define_StatRadarRangeMIN, _define_StatRadarRangeMAX);
		break;
	case BonusStatType::BonusMaxCPUPerfomance:			
		sMaxCompute = FMath::Clamp(sMaxCompute + value, _define_StatComputePerformanceMIN, _define_StatComputePerformanceMAX);
		break;
	case BonusStatType::BonusMaxPowerGridPerfomance:	
		sMaxPowerGrid = FMath::Clamp(sMaxPowerGrid + value, _define_StatPowerGridPerformanceMIN, _define_StatPowerGridPerformanceMAX);
		break;
	case BonusStatType::BonusMaxCargoSize:				
		sMaxCargo = FMath::Clamp(sMaxCargo + value, _define_StatCargoSizeMIN, _define_StatCargoSizeMAX);
		break;
	default:
		break;
	}
}

bool APlayerShip::LoadFromSave(const USaveLoader* loader) {

	if (sIsInited == false) {
		sCurrentShield = FMath::Clamp(loader->shield, _define_StatDamperMIN, _define_StatDamperMAX);
		sCurrentArmor = FMath::Clamp(loader->armor, _define_StatDamperMIN, _define_StatDamperMAX);
		sCurrentHull = FMath::Clamp(loader->hull, _define_StatDamperMIN, _define_StatDamperMAX);
		sCurrentPower = FMath::Clamp(loader->power, _define_StatDamperMIN, _define_StatDamperMAX);

		USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
		if (!IsValid(_tempInstance))
			return false;

		FItemData _tempModuleData;
		int _tempIndex = loader->slotTargetModule.Num();

		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][LoadFromSave] Currnet Target Module List : %d, Load Module List : %d"), slotTargetModule.Num(), loader->slotTargetModule.Num());
		for (int index = 0; index < slotTargetModule.Num(); index++) {
			if (index < _tempIndex && loader->slotTargetModule[index] > 0) {
				_tempModuleData = _tempInstance->GetItemData(loader->slotTargetModule[index]);
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
			}
		}

		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][LoadModuleInit] Currnet Active Module List : %d, Load Module List : %d"), slotActiveModule.Num(), loader->slotActiveModule.Num());
		_tempIndex = loader->slotActiveModule.Num();
		for (int index = 0; index < slotActiveModule.Num(); index++) {
			if (index < _tempIndex && loader->slotActiveModule[index] > 0) {
				_tempModuleData = _tempInstance->GetItemData(loader->slotActiveModule[index]);
				slotActiveModule[index].moduleID = _tempModuleData.ItemID;
				slotActiveModule[index].moduleType = _tempModuleData.ModuleType;
				slotActiveModule[index].maxUsagePower = _tempModuleData.MaxUsagePower;
				slotActiveModule[index].incrementUsagePower = _tempModuleData.IncrementUsagePower;
				slotActiveModule[index].decrementUsagePower = _tempModuleData.DecrementUsagePower;

				slotActiveModule[index].maxActiveModuleFactor = _tempModuleData.BonusStatValue;
				slotActiveModule[index].incrementActiveModuleFactor = _tempModuleData.IncrementActiveModuleFactor;
				slotActiveModule[index].decrementActiveModuleFactor = _tempModuleData.DecrementActiveModuleFactor;
			}
			else slotActiveModule[index] = FActiveModule();
		}
		int _tempMaxIndex = FMath::Min(loader->targetModuleAmmo.Num(), slotTargetModule.Num());
		for (int index = 0; index < slotTargetModule.Num(); index++) {
			if (index < _tempMaxIndex)
				slotTargetModule[index].ammo = loader->targetModuleAmmo[index];
			else slotTargetModule[index].ammo = FItem();
		}

		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][LoadModuleInit] Currnet Passive Module List : %d, Load Module List : %d"), slotPassiveModule.Num(), loader->slotPassiveModule.Num());
		_tempIndex = loader->slotPassiveModule.Num();
		for (int index = 0; index < slotPassiveModule.Num(); index++) {
			if (index < _tempIndex && loader->slotPassiveModule[index] > 0)
				slotPassiveModule[index] = loader->slotPassiveModule[index];
			else slotPassiveModule[index] = 0;
		}

		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][LoadModuleInit] Currnet SystemModules Module List : %d, Load Module List : %d"), slotSystemModule.Num(), loader->slotSystemModule.Num());
		_tempIndex = loader->slotSystemModule.Num();
		for (int index = 0; index < slotSystemModule.Num(); index++) {
			if (index < _tempIndex && loader->slotSystemModule[index] > 0)
				slotSystemModule[index] = loader->slotSystemModule[index];
			else slotSystemModule[index] = 0;
		}

		TotalStatsUpdate();
		sIsInited = true;
	}
	else return false;
	return true;
}

bool APlayerShip::EquipModule(const int moduleID) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempModuleData = _tempInstance->GetItemData(moduleID);
	if (!IsValid(_tempInstance))
		return false;

	if (behaviorState != BehaviorState::Docked) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Ship is not docked, so can't equip module."));
		return false;
	}

	if (sMaxCompute < CalculateCompute() + _tempModuleData.UsageCompute) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Lack of available Compute Perfomance."));
		return false;
	}
	if (sMaxPowerGrid < CalculatePowerGrid() + _tempModuleData.UsagePowerGrid) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Lack of available PowerGrid Output."));
		return false;
	}

	_tempModuleData = _tempInstance->GetItemData(moduleID);
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Try Equip item %d..."), _tempModuleData.ItemID);
	switch (_tempModuleData.Type) {
	case ItemType::TargetModule:
		for (int index = 0; index < slotTargetModule.Num(); index++) {
			if (slotTargetModule[index].moduleID < 1) {
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

				slotTargetModule[index].ammo = FItem(-1, 0);
				slotTargetModule[index].compatibleAmmo = _tempModuleData.UsageAmmo;
				slotTargetModule[index].ammoCapacity = _tempModuleData.AmmoCapacity;
				TotalStatsUpdate();
				for (int index1 = 0; index1 < slotTargetModule.Num(); index1++)
					UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Equip Finish : Current Target List - %d"), slotTargetModule[index1].moduleID);
				return true;
			}
			else continue;
		}
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Equip Fail : Target Slot Not enought"));
		break;
	case ItemType::ActiveModule:
		for (int index = 0; index < slotActiveModule.Num(); index++) {
			if (slotActiveModule[index].moduleID < 1) {
				slotActiveModule[index].moduleID = _tempModuleData.ItemID;
				slotActiveModule[index].moduleType = _tempModuleData.ModuleType;
				slotActiveModule[index].maxUsagePower = _tempModuleData.MaxUsagePower;
				slotActiveModule[index].incrementUsagePower = _tempModuleData.IncrementUsagePower;
				slotActiveModule[index].decrementUsagePower = _tempModuleData.DecrementUsagePower;

				slotActiveModule[index].maxActiveModuleFactor = _tempModuleData.BonusStatValue;
				slotActiveModule[index].incrementActiveModuleFactor = _tempModuleData.IncrementActiveModuleFactor;
				slotActiveModule[index].decrementActiveModuleFactor = _tempModuleData.DecrementActiveModuleFactor;
				TotalStatsUpdate();
				for (int index1 = 0; index1 < slotActiveModule.Num(); index1++)
					UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Equip Finish : Current Active List - %d"), slotActiveModule[index1].moduleID);
				return true;
			}
			else continue;
		}
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Equip Fail : Active Slot Not enought"));
		break;
	case ItemType::PassiveModule:
		for (int index = 0; index < slotPassiveModule.Num(); index++) {
			if (slotPassiveModule[index] < 1) {
				slotPassiveModule[index] = moduleID;
				TotalStatsUpdate();
				for (int index1 = 0; index1 < slotPassiveModule.Num(); index1++)
					UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Equip Finish : Current Passive List - %d"), slotPassiveModule[index1]);
				return true;
			}
			else continue;
		}
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Equip Fail : Passive Slot Not enought"));
		break;
	case ItemType::SystemModule:
		for (int index = 0; index < slotSystemModule.Num(); index++) {
			if (slotSystemModule[index] < 1) {
				slotSystemModule[index] = moduleID;
				TotalStatsUpdate();
				for (int index1 = 0; index1 < slotSystemModule.Num(); index1++)
					UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Equip Finish : Current System List - %d"), slotSystemModule[index1]);
				return true;
			}
			else continue;
		}
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Equip Fail : System Slot Not enought"));
		break;
	default:
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Equip Fail : this item is not module."));
		break;
	}

	UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][EquipModule] Equip Conditional Check Fail."));
	return false;
}

bool APlayerShip::UnEquipModule(const ItemType moduleItemType, const int slotNumber) {
	if (behaviorState != BehaviorState::Docked) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Ship is not docked, so can't unequip module."));
		return false;
	}

	AUserState* userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
	switch (moduleItemType)
	{
	case ItemType::TargetModule:
		if (slotTargetModule.Num() <= slotNumber || slotTargetModule.Num() < 1 || slotTargetModule[slotNumber].moduleID <= 0)
			return false;
		if (!userState->AddPlayerCargo(slotTargetModule[slotNumber].moduleID))
			return false;

		switch (slotTargetModule[slotNumber].moduleType) {
		case ModuleType::Beam:
		case ModuleType::TractorBeam:
		case ModuleType::MinerBeam:
			slotTargetModule[slotNumber].moduleID = -1;
			slotTargetModule[slotNumber].moduleType = ModuleType::NotModule;
			break;
		case ModuleType::Cannon:
		case ModuleType::Railgun:
		case ModuleType::MissileLauncher:
			if (userState->AddPlayerCargo(slotTargetModule[slotNumber].ammo)) {
				slotTargetModule[slotNumber].moduleID = -1;
				slotTargetModule[slotNumber].moduleType = ModuleType::NotModule;
			}
			else {
				userState->DropPlayerCargo(slotTargetModule[slotNumber].moduleID);
				return false;
			}
			break;
		default:
			break;
		}
		break;
	case ItemType::ActiveModule:
		if (slotActiveModule.Num() <= slotNumber || slotActiveModule.Num() < 1 || slotActiveModule[slotNumber].moduleID <= 0)
			return false;
		slotActiveModule[slotNumber].moduleID = -1;
		slotActiveModule[slotNumber].moduleType = ModuleType::NotModule;
		break;
	case ItemType::PassiveModule:
		if (slotPassiveModule.Num() <= slotNumber || slotPassiveModule.Num() < 1 || slotPassiveModule[slotNumber] <= 0)
			return false;
		slotPassiveModule[slotNumber] = -1;
		break;
	case ItemType::SystemModule:
		if (slotSystemModule.Num() <= slotNumber || slotSystemModule.Num() < 1 || slotSystemModule[slotNumber] <= 0)
			return false;
		slotSystemModule[slotNumber] = -1;
		break;
	default:
		break;
	}
	TotalStatsUpdate();
	return true;
}

void APlayerShip::GetModule(const ItemType moduleType, TArray<int>& moduleList) const {
	switch (moduleType) {
	case ItemType::TargetModule:
		moduleList.Init(-1, slotTargetModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) {
			if (slotTargetModule[index].moduleType != ModuleType::NotModule)
				moduleList[index] = slotTargetModule[index].moduleID;
		}
		break;
	case ItemType::ActiveModule:
		moduleList.Init(-1, slotActiveModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) {
			if (slotActiveModule[index].moduleType != ModuleType::NotModule)
				moduleList[index] = slotActiveModule[index].moduleID;
		}
		break;
	case ItemType::PassiveModule:
		moduleList.Init(-1, slotPassiveModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) 
			moduleList[index] = slotPassiveModule[index];
		break;
	case ItemType::SystemModule:
		moduleList.Init(-1, slotSystemModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) 
			moduleList[index] = slotSystemModule[index];
		break;
	default:
		break;
	}
}

void APlayerShip::GetModuleActivate(const ItemType moduleType, TArray<float>& moduleActivate) const {

	switch (moduleType) {
	case ItemType::TargetModule:
		moduleActivate.Init(0, slotTargetModule.Num());
		for (int index = 0; index < moduleActivate.Num(); index++) {
			if (slotTargetModule[index].moduleState != ModuleState::NotActivate)
				moduleActivate[index] = slotTargetModule[index].remainCooltime / (FMath::Max(1.0f, slotTargetModule[index].maxCooltime));
		}
		break;
	case ItemType::ActiveModule:
		moduleActivate.Init(0, slotActiveModule.Num());
		for (int index = 0; index < moduleActivate.Num(); index++) {
			if (slotActiveModule[index].moduleState != ModuleState::NotActivate)
				moduleActivate[index] = 1.0f;
		}
		break;
	default:
		return;
	}
}

void APlayerShip::GetTargetModuleAmmo(TArray<FItem>& targetModuleAmmo) const {

	targetModuleAmmo.Init(0, slotTargetModule.Num());
	for (int index = 0; index < targetModuleAmmo.Num(); index++) 
		targetModuleAmmo[index] = slotTargetModule[index].ammo;
}

/*
* 타게팅 모듈의 동작을 제어. 플레이어 전용.
* @param slotIndex - 타게팅 모듈 슬롯 번호.
* @return 처리 후의 모듈 동작 상태.
*/
bool APlayerShip::ToggleTargetModule(const int slotIndex, ASpaceObject* target) {

	//타게팅 모듈에 한해서만( < ModuleType::ShieldGenerator ) 함수 처리
	if (slotIndex < slotTargetModule.Num() && slotTargetModule[slotIndex].moduleType > ModuleType::NotModule && slotTargetModule[slotIndex].moduleType < ModuleType::ShieldGenerator) {
		//모듈의 예상 소모 전력보다 낮은 수준의 전력을 보유하였다면 활성화 거부
		if (slotTargetModule[slotIndex].maxUsagePower * slotTargetModule[slotIndex].maxCooltime > sCurrentPower)
			return false;

		//현재 모듈이 활성화되어 있을 경우 -> 작동 중지 예약, 현재 켜져있는 상태임을 리턴
		if (slotTargetModule[slotIndex].moduleState != ModuleState::NotActivate) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : Request Off."), slotIndex);
			slotTargetModule[slotIndex].isBookedForOff = true;
			return true;
		}
		else {
			if (!IsValid(target) || target == this) {
				UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Can't find target. current State : %d"), (int)(slotTargetModule[slotIndex].moduleState));
				return false;
			}

			//현재 꺼져있는 상태
			switch (slotTargetModule[slotIndex].moduleType) {
			case ModuleType::Beam:
			case ModuleType::TractorBeam:
			case ModuleType::MinerBeam:
				//모듈이 빔 계열인 경우
				if (!IsValid(target))
					return false;
				slotTargetModule[slotIndex].moduleState = ModuleState::Activate;
				slotTargetModule[slotIndex].isBookedForOff = false;
				slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
				slotTargetModule[slotIndex].target = target;
				UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : On. target is %s."), slotIndex, *target->GetName());
				return true;
				break;
			case ModuleType::Cannon:
			case ModuleType::Railgun:
			case ModuleType::MissileLauncher:
				//모듈이 실탄 계열인 경우, 모듈을 활성화하기 이전에 ammo 상태를 확인
				if (slotTargetModule[slotIndex].ammo.itemAmount < 1) {
					AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
					TArray<FItem> _tempItemArray;

					//ammo를 카고 리스트에서 찾았음.
					if (_userState->FindItemAmount(slotTargetModule[slotIndex].ammo.itemID) > 0) {
						UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - reload : On."), slotIndex);
						slotTargetModule[slotIndex].moduleState = ModuleState::ReloadAmmo;
						slotTargetModule[slotIndex].target = target;
						slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
					} else
						UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : Not On. Ammo not enoght."), slotIndex);
				} else if (IsValid(target)) {
					slotTargetModule[slotIndex].moduleState = ModuleState::Activate;
					slotTargetModule[slotIndex].target = target;

					slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
					UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : On. target is %s."), slotIndex, *target->GetName());
				} else
					UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : Not On. target is Null."), slotIndex);
				slotTargetModule[slotIndex].isBookedForOff = false;
				return true;
				break;
			default:
				return false;
				break;
			}
		}
	}
	UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle Fail. forced OFF."), slotIndex);
	for (int index = 0; index < slotTargetModule.Num(); index++)
		slotTargetModule[index].moduleState = ModuleState::NotActivate;
	return false;
}

/*
* 타게팅 모듈의 탄을 변경. 플레이어 전용.
* @param slotIndex - 타게팅 모듈 슬롯 번호.
* @param selectedAmmoID - 변경할 탄의 ID.
*/
void APlayerShip::SettingAmmo(const int selectedAmmoID) {

	AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
	if (!IsValid(_userState))
		return;
	if (_userState->FindItemAmount(selectedAmmoID) < 1)
		return;

	for (FTargetModule& targetModule : slotTargetModule) {
		for (int& id : targetModule.compatibleAmmo) {
			if (id == selectedAmmoID) {
				if (!_userState->AddPlayerCargo(targetModule.ammo))
					return;

				targetModule.ammo = FItem(selectedAmmoID, 0);
				targetModule.isBookedForOff = true;
				break;
			}
			
		}
	}
}

/*
* 액티브 모듈의 동작을 제어.
* @param slotIndex - 타게팅 모듈 슬롯 번호.
* @return 처리 후의 모듈 동작 상태.
*/
bool APlayerShip::ToggleActiveModule(const int slotIndex) {
	if (slotIndex < slotActiveModule.Num() && slotActiveModule[slotIndex].moduleType > ModuleType::HullRepairLaser && slotActiveModule[slotIndex].moduleType < ModuleType::PassiveModule) {
		//모듈의 예상 소모 전력보다 낮은 수준의 전력을 보유하였다면 활성화 거부
		if (slotActiveModule[slotIndex].maxUsagePower > sCurrentPower)
			return false;

		slotActiveModule[slotIndex].moduleState = (slotActiveModule[slotIndex].moduleState != ModuleState::NotActivate ? ModuleState::NotActivate : ModuleState::Activate);
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Active Module %d - Toggle : %s"),
			slotIndex, slotActiveModule[slotIndex].moduleState != ModuleState::NotActivate ? TEXT("ON") : TEXT("OFF"));
		return slotActiveModule[slotIndex].moduleState != ModuleState::NotActivate ? true : false;
	}

	UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Active Module - Toggle Fail. forced OFF to init."));
	for (int index = 0; index < slotActiveModule.Num(); index++)
		slotActiveModule[index].moduleState = ModuleState::NotActivate;
	return false;
}

UCameraComponent* APlayerShip::GetCamera() {
	return playerViewpointCamera;
}

void APlayerShip::ControlViewPointX(const float value) {
	//playerViewpointCamera->AddWorldOffset(FVector(0.0f, value * _define_CamZoomFactor, 0.0f));
}

void APlayerShip::ControlViewPointY(const float value) {
	//playerViewpointCamera->AddWorldOffset(FVector(-value * _define_CamZoomFactor, 0.0f, 0.0f));
}

void APlayerShip::ControlCamDistance(const float value) {
	if (!playerViewpointCamera)
		return;
	playerViewpointCamera->OrthoWidth
		= FMath::Clamp(playerViewpointCamera->OrthoWidth * (1.0f + value), _define_CameraDinstanceMIN, _define_CameraDinstanceMAX);
}

void APlayerShip::ControlViewPointOrigin() {
	if (!playerViewpointCamera)
		return;
	playerViewpointCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

void APlayerShip::SetTargetSpeed(const float value) {
	targetSpeed = FMath::Clamp(value, 0.0f, 1.0f);
}

void APlayerShip::SetAcceleration(const float value) {
	accelerationFactor = FMath::Clamp(value, 0.0f, 1.0f);
}

void APlayerShip::SetRotateRate(const float value) {
	rotateRateFactor = FMath::Clamp(value, 0.0f, 1.0f);
}
#pragma endregion

#pragma region Interface Implementing : ICommandable
void APlayerShip::CommandStop() {
	if (CheckCanBehavior() == true) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandStop] Receive Command Stop!"));
		behaviorState = BehaviorState::Idle;
		setedTargetSpeed = 0.0f;
		targetRotateRateFactor = 0.0f;
	}
}

bool APlayerShip::CommandMoveToPosition(FVector position) {
	if (CheckCanBehavior() == true) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandMove] Receive Command Move! : %.2f, %.2f, %.2f"), position.X, position.Y, position.Z);

		moveTargetVector = position;
		targetObject = nullptr;
		behaviorState = BehaviorState::Move;
		return true; 
	}
	else return false;
}

bool APlayerShip::CommandMoveToTarget(ASpaceObject* target) {

	if (CheckCanBehavior() == true) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandMove] Receive Command Move! : %s"), *target->GetName());
		targetObject = target;
		behaviorState = BehaviorState::Move; 
		return true; 
	}
	else return false;
}

bool APlayerShip::CommandAttack(ASpaceObject* target) {

	return false;
}

bool APlayerShip::CommandMining(AResource* target) {
	
	if (CheckCanBehavior() == true) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandMining] Receive Command Mining! : %s"), *target->GetName());
		targetObject = target;
		behaviorState = BehaviorState::Mining;
		return true;
	}
	else return false;
}

bool APlayerShip::CommandRepair(ASpaceObject* target) {
	
	if (CheckCanBehavior() == true) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandRepair] Receive Command Repair! : %s"), *target->GetName());
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

bool APlayerShip::CommandJump(TScriptInterface<IStructureable> target) {
	
	if (!CheckCanBehavior() || target == nullptr || !target.GetObjectRef()->IsA(AGate::StaticClass()))
		return false;

	if (this->GetDistanceTo(Cast<AActor>(target.GetObjectRef())) < _define_AvailableDistanceToJump) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandJump] Receive Command Jump! : %s"), *target->GetDestinationName());
		Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->Jump(target->GetDestinationName());
		return true;
	} else {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandJump] Too Far! Can't Jump : %s"), *target->GetDestinationName());
		return true;
	}
}

bool APlayerShip::CommandWarp(FVector location) {
	
	if (CheckCanBehavior() == true) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandWarp] Receive Command Warp! : %.2f, %.2f, %.2f"), location.X, location.Y, location.Z);
		SetActorLocation(location, false, nullptr, ETeleportType::TeleportPhysics);
		behaviorState = BehaviorState::Idle;
		return true;
	}
	else return false;
}

bool APlayerShip::CommandDock(TScriptInterface<IStructureable> target) {
	
	if (CheckCanBehavior() == true && target.GetObjectRef()->IsA(ASpaceObject::StaticClass())) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandDock] Receive Command Dock! Docking to Station Request : %s"), *target.GetObjectRef()->GetName());
		if (target->RequestedDock(faction, shipClass, dockingLocation, dockingRotation)) {
			targetStructure = target;
			targetObject = Cast<ASpaceObject>(target.GetObjectRef());
			moveTargetVector = targetObject->GetActorLocation() + dockingRotation.RotateVector(dockingLocation);
			behaviorState = BehaviorState::Docking;
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandDock] Docking to Station Accepted. Start Sequence Dock."));
			return true;
		}
		else {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandDock] Docking to Station Rejected. Order Cancel."));
			return false;
		}
	}
	return false;
}

bool APlayerShip::CommandUndock() {
	if (behaviorState == BehaviorState::Docked) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandUndock] Receive Command Undock!"));
		Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->SetDockedStructure(nullptr);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OffUIStationButton();
		targetObject = nullptr;
		behaviorState = BehaviorState::Idle;
		return true;
	}
	else return false;
}

bool APlayerShip::CommandLaunch(const TArray<int>& baySlot) {
	if (CheckCanBehavior() == true) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandLaunch] Receive Command Launch! : %d"), baySlot.Num());
		return true;
	}
	else return false;
}
#pragma endregion

#pragma region Functions
bool APlayerShip::MoveDistanceCheck() {
	if (IsValid(targetObject) && targetObject->GetObjectType() != ObjectType::Station && targetObject->GetObjectType() != ObjectType::Gate)
		moveTargetVector = targetObject->GetActorLocation();

	remainDistance = FVector::Dist(moveTargetVector, GetActorLocation());
	moveTargetVector.Z = 0.0f;
	//checks distance and Angle For start Acceleration.
	if (remainDistance > (FMath::Pow(currentSpeed, 2) / FMath::Clamp(sMinAcceleration * accelerationFactor * 2.0f, 1.0f, 9999.0f) + 5.0f)) {
		if (FMath::Abs(moveTargetRotate.Yaw) < sStartAccelAngle)
			setedTargetSpeed = targetSpeed * sMaxSpeed * (1.0f + moduleStatEngine);
		else setedTargetSpeed = 0.0f;
	}
	else  {
		setedTargetSpeed = 0.0f;
		return true;
	}
	//arrive to Destination not yet.
	return false;
}

void APlayerShip::RotateCheck() {

	moveTargetVector.Z = 0;
	moveTargetRotate = (moveTargetVector - GetActorLocation()).Rotation() - GetActorRotation();
	RotationDot = FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(GetActorForwardVector(), moveTargetVector - GetActorLocation()));

	if (RotationDot > 0.01f) {
		if (FMath::Abs(moveTargetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(sRotateDeceleration * rotateRateFactor * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor;
		else
			targetRotateRateFactor = 0.0f;
	}
	else if (RotationDot < -0.01f) {
		if (FMath::Abs(moveTargetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(sRotateDeceleration * rotateRateFactor * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = -sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor;
		else
			targetRotateRateFactor = 0.0f;
	}
	else targetRotateRateFactor = 0.0f;
}

void APlayerShip::Movement() {
	if (targetRotateRateFactor > 0.0f) {
		if (realRotateRateFactor >= 0.0f) {
			if (targetRotateRateFactor > realRotateRateFactor)
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * sRotateAcceleration * tempDeltaTime,
					0.0f, targetRotateRateFactor);
			else
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * sRotateDeceleration * tempDeltaTime,
					targetRotateRateFactor, sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor);
		}
		else {
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * sRotateDeceleration * tempDeltaTime,
				-sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor, 0.0f);
		}
	}
	else if (targetRotateRateFactor < 0.0f) {
		if (realRotateRateFactor <= 0.0f) {
			if (targetRotateRateFactor > realRotateRateFactor)
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * sRotateDeceleration * tempDeltaTime,
					-sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor, targetRotateRateFactor);
			else
				realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * sRotateAcceleration * tempDeltaTime,
					targetRotateRateFactor, 0.0f);
		}
		else {
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * sRotateDeceleration * tempDeltaTime,
				0.0f, sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor);
		}
	}
	else {
		if (realRotateRateFactor < 0.0f)
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor + rotateRateFactor * sRotateDeceleration * tempDeltaTime,
				-sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor, 0.0f);
		else
			realRotateRateFactor = FMath::Clamp(realRotateRateFactor - rotateRateFactor * sRotateDeceleration * tempDeltaTime,
				0.0f, sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor);
	}

	if (currentSpeed < setedTargetSpeed)
		currentSpeed = FMath::Clamp(currentSpeed + accelerationFactor * sMaxAcceleration * (1.0f + moduleStatAcceleration) * tempDeltaTime,
			0.0f, setedTargetSpeed);
	else if (currentSpeed > setedTargetSpeed)
		currentSpeed = FMath::Clamp(currentSpeed - accelerationFactor * sMinAcceleration * (1.0f + moduleStatAcceleration) * tempDeltaTime,
			0.0f, sMaxSpeed * (1.0f + moduleStatEngine));

	AddActorLocalRotation(FRotator(0.0f, realRotateRateFactor, 0.0f) * tempDeltaTime);
	AddActorWorldOffset(GetActorForwardVector() * currentSpeed * tempDeltaTime, true);
}

void APlayerShip::ModuleCheck() {
	moduleStatShieldRegen = moduleStatArmorRepair = moduleStatHullRepair = 0.0f;
	moduleStatEngine = moduleStatAcceleration = moduleStatThruster = 0.0f;
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
		if (sCurrentPower < 5.0f || !IsValid(module.target))
			module.isBookedForOff = true;

		//모듈이 켜져있을 경우 power 소모량 증가 및 쿨타임 지속 감소
		if (module.moduleState != ModuleState::NotActivate) {
			module.currentUsagePower = FMath::Clamp(module.currentUsagePower + module.incrementUsagePower * _define_ModuleANDPathTick,
				0.0f, module.maxUsagePower);
			module.remainCooltime = FMath::Clamp(module.remainCooltime - _define_ModuleANDPathTick, 0.0f, FMath::Max(1.0f, module.maxCooltime));

			//쿨타임 완료시 행동 실시
			if (module.remainCooltime <= 0.0f) {
				if (module.isBookedForOff)
					module.moduleState = ModuleState::NotActivate;
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
						_spawnedTransform = FTransform(_targetedRotation, _launchLocation);
						//빔계열 모듈의 경우
						if (module.moduleType == ModuleType::Beam ||
							module.moduleType == ModuleType::MinerBeam ||
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
						else if (module.moduleType > ModuleType::Beam && module.moduleType < ModuleType::MinerBeam) {
							if (module.ammo.itemAmount < 1) {
								module.moduleState = ModuleState::ReloadAmmo;
								module.isBookedForOff = false;
							} else {
								module.ammo.itemAmount--;
								AProjectiles* _projectile = Cast<AProjectiles>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AProjectiles::StaticClass(),
									_spawnedTransform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
								if (!IsValid(_projectile))
									return;
								switch (module.moduleType) {
								case ModuleType::Cannon:
								case ModuleType::Railgun:
									_projectile->SetProjectileProperty(module.ammo.itemID, this,
										module.damageMultiple, module.launchSpeedMultiple, module.ammoLifeSpanBonus);
									break;
								case ModuleType::MissileLauncher:
									_projectile->SetActorRotation(_launchRotation);
									_projectile->SetProjectileProperty(module.ammo.itemID, this,
										module.damageMultiple, module.launchSpeedMultiple, module.ammoLifeSpanBonus, module.target);
									break;
								}
								UGameplayStatics::FinishSpawningActor(_projectile, _spawnedTransform);
							}
						}
					} 
					else {
						module.moduleState = ModuleState::NotActivate;
						module.target = nullptr;
						module.isBookedForOff = false;
					}
					break;
				case ModuleState::ReloadAmmo:
					//캐논, 레일건, 미사일류 모듈일 경우
					if (module.moduleType > ModuleType::Beam && module.moduleType < ModuleType::MinerBeam) {

						AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
						int _reloadedAmount = _userState->FindItemAmount(module.ammo.itemID);
						
						//ammo를 찾지 못하였음. 재장전 및 모듈 동작 실시하지 않음.
						if (_reloadedAmount < 1)
							module.moduleState = ModuleState::NotActivate;
						//ammo를 찾음. 재장전 실시. 모듈 동작은 재장전이 완료되면 자동으로 시작.
						else {
							_reloadedAmount = FMath::Min(_reloadedAmount, module.ammoCapacity - module.ammo.itemAmount);
							if (_userState->DropPlayerCargo(FItem(module.ammo.itemID, _reloadedAmount))) {
								UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Reload Start!"));
								module.ammo.itemAmount = _reloadedAmount;
								if (IsValid(module.target)) {
									module.moduleState = ModuleState::Activate;
									UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Reload Finish! and Target On!"));
								} else {
									module.moduleState = ModuleState::NotActivate;
									UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Reload Finish!"));
								}
							} else {
								//ammo를 찾았으나 장전에 실패.
								UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Reload Fail!"));
								module.moduleState = ModuleState::NotActivate;
							}
						}
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
					module.target = nullptr;
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
	for (FActiveModule& module : slotActiveModule) {
		if (sCurrentPower < 5.0f || module.moduleType < ModuleType::ShieldGenerator || module.moduleType > ModuleType::SteeringController)
			module.moduleState = ModuleState::NotActivate;
		if (module.moduleState == ModuleState::Activate) {
			module.currentUsagePower = FMath::Clamp(module.currentUsagePower + module.incrementUsagePower * _define_ModuleANDPathTick,
				0.0f, module.maxUsagePower);
			module.currentActiveModuleFactor = FMath::Clamp(module.currentActiveModuleFactor + module.incrementActiveModuleFactor * _define_ModuleANDPathTick,
				0.0f, module.maxActiveModuleFactor);
		}
		else {
			module.currentUsagePower = FMath::Clamp(module.currentUsagePower - module.decrementUsagePower * _define_ModuleANDPathTick,
				0.0f, module.maxUsagePower);
			module.currentActiveModuleFactor = FMath::Clamp(module.currentActiveModuleFactor - module.decrementActiveModuleFactor * _define_ModuleANDPathTick,
				0.0f, module.maxActiveModuleFactor);
		}
		moduleConsumptPower += module.currentUsagePower;
		switch (module.moduleType) {
		case ModuleType::ShieldGenerator:	moduleStatShieldRegen += module.currentActiveModuleFactor;		break;
		case ModuleType::ArmorRepairer:		moduleStatArmorRepair += module.currentActiveModuleFactor;		break;
		case ModuleType::HullRepairer:		moduleStatHullRepair += module.currentActiveModuleFactor;		break;
		case ModuleType::EngineController:	moduleStatEngine += module.currentActiveModuleFactor;			break;
		case ModuleType::Accelerator:		moduleStatAcceleration += module.currentActiveModuleFactor;	break;
		case ModuleType::SteeringController:moduleStatThruster += module.currentActiveModuleFactor;		break;
		default:	break;
		}
	}
}

bool APlayerShip::CheckCanBehavior() const {
	switch (behaviorState)
	{
	case BehaviorState::Docked:
	case BehaviorState::Warping:
		return false;
	default:
		return true;
	}
}

const float APlayerShip::CalculateCompute() const {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempModuleData;
	float _result = 0;
	if (!IsValid(_tempInstance))
		return _result;

	for (int index = 0; index < slotTargetModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotTargetModule[index].moduleID);
		_result += _tempModuleData.UsageCompute;
	}
	for (int index = 0; index < slotActiveModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotActiveModule[index].moduleID);
		_result += _tempModuleData.UsageCompute;
	}
	for (int index = 0; index < slotPassiveModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotPassiveModule[index]);
		_result += _tempModuleData.UsageCompute;
	}
	for (int index = 0; index < slotSystemModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotSystemModule[index]);
		_result += _tempModuleData.UsageCompute;
	}
	return _result;
}

const float APlayerShip::CalculatePowerGrid() const {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempModuleData;
	float _result = 0;
	if (!IsValid(_tempInstance))
		return _result;

	for (int index = 0; index < slotTargetModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotTargetModule[index].moduleID);
		_result += _tempModuleData.UsagePowerGrid;
	}
	for (int index = 0; index < slotActiveModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotActiveModule[index].moduleID);
		_result += _tempModuleData.UsagePowerGrid;
	}
	for (int index = 0; index < slotPassiveModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotPassiveModule[index]);
		_result += _tempModuleData.UsagePowerGrid;
	}
	for (int index = 0; index < slotSystemModule.Num(); index++)
	{
		_tempModuleData = _tempInstance->GetItemData(slotSystemModule[index]);
		_result += _tempModuleData.UsagePowerGrid;
	}
	return _result;
}
#pragma endregion

