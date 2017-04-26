// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "PlayerShip.h"

APlayerShip::APlayerShip()
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
	
	playerViewpointArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FixArm"));
	playerViewpointCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FixedCamera"));
	playerViewpointArm->SetupAttachment(RootComponent, RootComponent->GetAttachSocketName());
	playerViewpointCamera->SetupAttachment(playerViewpointArm, playerViewpointArm->GetAttachSocketName());

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0f;

	sShipID = -1;
	checkTime = 0.0f;
	sIsInited = false;
}

#pragma region Event Calls
void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	if (Cast<APawn>(this) != UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][Begin] Spawn Fail! this is not User's Ship!"));
		Destroy();
	}
	traceParams = FCollisionQueryParams(FName("PathFind"), true, this);

	playerViewpointArm->AddWorldRotation(FRotator(-45.0f, 0.0f, 0.0f));
	playerViewpointArm->CameraLagMaxDistance = 1000.0f;
	playerViewpointArm->bEnableCameraRotationLag = true;
	playerViewpointArm->bEnableCameraLag = true;
	playerViewpointArm->CameraLagSpeed = 100.0f;
	playerViewpointArm->CameraRotationLagSpeed = 7.0f;
	playerViewpointArm->bAbsoluteRotation = true;
	playerViewpointArm->bDoCollisionTest = false;
	playerViewpointArm->bUsePawnControlRotation = false;

	sShipID = -1;
	checkTime = 0.0f;
	sIsInited = false;
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][Begin] Spawn Finish!"));
}

void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	tempDeltaTime = DeltaTime;
	checkTime += DeltaTime;
	if (checkTime > 0.5f) {
		checkTime = 0.0f;

		CheckPath();
		ModuleCheck();
	}

	sCurrentHull = FMath::Clamp(sCurrentHull + FMath::Clamp((sRepairHull + moduleStatHullRepair), 0.0f, 500.0f) * DeltaTime, 0.0f, sMaxHull);
	sCurrentArmor = FMath::Clamp(sCurrentArmor + FMath::Clamp((sRepairArmor + moduleStatArmorRepair), 0.0f, 500.0f) * DeltaTime, 0.0f, sMaxArmor);
	sCurrentShield = FMath::Clamp(sCurrentShield + FMath::Clamp((sRechargeShield + moduleStatShieldRegen), 0.0f, 500.0f) * DeltaTime, 0.0f, sMaxShield);
	sCurrentPower = FMath::Clamp(sCurrentPower + FMath::Clamp(sRechargePower - moduleConsumptPower, -500.0f, 500.0f) * DeltaTime, 0.0f, sMaxPower);

	playerViewpointArm->TargetArmLength = FMath::Clamp(playerViewpointArm->TargetArmLength + SmoothZoomRemain * 0.05f, 100.0f, playerViewpointArm->CameraLagMaxDistance);
	SmoothZoomRemain -= SmoothZoomRemain * 0.75f * DeltaTime;

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
			targetObject = nullptr;
			bIsStraightMove = true;
			moveTargetVector = Cast<AActor>(targetStructure.GetObjectRef())->GetActorForwardVector() * 1000.0f + GetActorLocation();
			behaviorState = BehaviorState::Docked;
			Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->SetDockedStructure(targetStructure);
			Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OnUIStationButton();
		}
		break;
	case BehaviorState::Battle:
		break;
	case BehaviorState::Warping:
		break;
	default:
		break;
	}
	RotateCheck();
	Movement();
}

float APlayerShip::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	Faction dealingFaction;

	if (DamageCauser->IsA(ABeam::StaticClass()))
		dealingFaction = Cast<ABeam>(DamageCauser)->GetLaunchingFaction();
	else if (DamageCauser->IsA(AProjectiles::StaticClass()))
		dealingFaction = Cast<AProjectiles>(DamageCauser)->GetLaunchingFaction();
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
	bool _isCritical = false;

	//Hited된 방향으로 크리티컬 계산.
	//Front : multiply x2
	//back : multiply x3
	if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) > 0.95f) {
		_remainDamage *= 2.0f;
		_isCritical = true;
	}
	else if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) < -0.95f) {
		_remainDamage *= 3.0f;
		_isCritical = true;
	}

	//방어 및 데미지 계산. 방어체계 소진시 남은 데미지량은 다음 방어체계로.
	//remainDamage = sDefShield;
	if (sCurrentShield > _remainDamage) {
		_effectShieldDamage = _remainDamage;
		sCurrentShield = sCurrentShield - _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectShieldDamage = sCurrentShield;
		_remainDamage -= _effectShieldDamage;
		sCurrentShield = 0.0f;
	}

	if (sCurrentArmor > _remainDamage) {
		_effectArmorDamage = _remainDamage;
		sCurrentArmor = sCurrentArmor - _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectArmorDamage = sCurrentArmor;
		_remainDamage -= _effectArmorDamage;
		sCurrentArmor = 0.0f;
	}

	if (sCurrentHull > _remainDamage) {
		_effectHullDamage = _remainDamage;
		sCurrentHull = sCurrentHull - _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectHullDamage = sCurrentHull;
		sCurrentHull = 0.0f;
		UGameplayStatics::OpenLevel(GetWorld(), "MainTitle", TRAVEL_Absolute);
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Drone][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage, _isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void APlayerShip::SetupPlayerInputComponent(class UInputComponent* inputComponent)
{
	Super::SetupPlayerInputComponent(inputComponent);
}

void APlayerShip::BeginDestroy() {
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int APlayerShip::GetObjectID() {
	return sShipID;
}

ObjectType APlayerShip::GetObjectType() {
	return ObjectType::Ship;
}

Faction APlayerShip::GetFaction() {
	return Faction::Player;
}

void APlayerShip::SetFaction(Faction setFaction) {
	return;
}

BehaviorState APlayerShip::GetBehaviorState() {
	return behaviorState;
}

bool APlayerShip::InitObject(int objectId) {
	if (sIsInited == true && (objectId < 0 || objectId == sShipID))
		return false;

	UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][InitObject] before init ID : %d, Init to ID : %d"), sShipID, objectId);

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FShipData _tempShipData = _tempInstance->GetShipData(objectId);

	if (sShipID != objectId) {
		sShipID = objectId;
		objectName = _tempShipData.Name;
		UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempShipData.MeshPath.ToString()));
		if (newMesh) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][InitObject] Mesh Generate!"));
			objectMesh->SetStaticMesh(newMesh);
		}

		slotTargetModule.SetNum(FMath::Clamp(_tempShipData.SlotTarget, 0, 8));
		targetingObject.SetNum(FMath::Clamp(_tempShipData.SlotTarget, 0, 8));
		slotActiveModule.SetNum(FMath::Clamp(_tempShipData.SlotActive, 0, 8));
		slotPassiveModule.SetNum(FMath::Clamp(_tempShipData.SlotPassive, 0, 8));
		slotSystemModule.SetNum(FMath::Clamp(_tempShipData.SlotSystem, 0, 8));

		slotTargetModule.Init(FTargetModule(), FMath::Clamp(_tempShipData.SlotTarget, 0, 8));
		targetingObject.Init(nullptr, FMath::Clamp(_tempShipData.SlotTarget, 0, 8));
		slotActiveModule.Init(FActiveModule(), FMath::Clamp(_tempShipData.SlotActive, 0, 8));
		slotPassiveModule.Init(0, FMath::Clamp(_tempShipData.SlotPassive, 0, 8));
		slotSystemModule.Init(0, FMath::Clamp(_tempShipData.SlotSystem, 0, 8));
		
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][InitObject] Init Module Slots : %d, %d, %d, %d"), slotTargetModule.Num(), slotActiveModule.Num(), slotPassiveModule.Num(), slotSystemModule.Num());

		lengthToLongAsix = FMath::Clamp(_tempShipData.lengthToLongAsix, 10.0f, 10000.0f);
		lengthRader = FMath::Clamp(_tempShipData.lengthRader, 10.0f, 100000.0f);
	}

	sMaxShield = FMath::Clamp(_tempShipData.Shield, 10.0f, 10000000.0f);
	sRechargeShield = FMath::Clamp(_tempShipData.RechargeShield, 0.0f, 500.0f);
	sDefShield = FMath::Clamp(_tempShipData.DefShield, 0.0f, 1000.0f);

	sMaxArmor = FMath::Clamp(_tempShipData.Armor, 10.0f, 10000000.0f);
	sRepairArmor = FMath::Clamp(_tempShipData.RepairArmor, 0.0f, 500.0f);
	sDefArmor = FMath::Clamp(_tempShipData.DefArmor, 0.0f, 1000.0f);

	sMaxHull = FMath::Clamp(_tempShipData.Hull, 10.0f, 10000000.0f);
	sRepairHull = FMath::Clamp(_tempShipData.RepairHull, 0.0f, 500.0f);
	sDefHull = FMath::Clamp(_tempShipData.DefHull, 0.0f, 1000.0f);

	sMaxPower = FMath::Clamp(_tempShipData.Power, 10.0f, 10000000.0f);
	sRechargePower = FMath::Clamp(_tempShipData.RechargePower, 0.0f, 500.0f);

	sMaxCompute = FMath::Clamp(_tempShipData.Compute, 0.0f, 100000.0f);
	sMaxPowerGrid = FMath::Clamp(_tempShipData.PowerGrid, 0.0f, 100000.0f);
	sMaxCargo = FMath::Clamp(_tempShipData.Cargo, 50.0f, 1000000.0f);

	sMaxSpeed = FMath::Clamp(_tempShipData.MaxSpeed, 0.0f, 10000.0f);
	sMinAcceleration = FMath::Clamp(_tempShipData.MinAcceleration, 0.0f, 1000.0f);
	sMaxAcceleration = FMath::Clamp(_tempShipData.MaxAcceleration, 0.0f, 1000.0f);
	sStartAccelAngle = FMath::Clamp(_tempShipData.StartAccelAngle, 0.0f, 180.0f);

	sMaxRotateRate = FMath::Clamp(_tempShipData.MaxRotateRate, 0.0f, 90.0f);
	sRotateAcceleration = FMath::Clamp(_tempShipData.rotateAcceleraion, 0.0f, 90.0f);
	sRotateDeceleration = FMath::Clamp(_tempShipData.rotateDeceleraion, 0.0f, 90.0f);

	if (TotalStatsUpdate() == false)
		return false;
	
	return true;
}

bool APlayerShip::LoadBaseObject(float shield, float armor, float hull, float power) {
	return false;
}

float APlayerShip::GetValue(GetStatType statType) {
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

	case GetStatType::maxSpeed:
		_value = sMaxSpeed;
		break;
	case GetStatType::targetSpeed:
		_value = setedTargetSpeed;
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

void APlayerShip::GetRepaired(GetStatType statType, float repairValue) {

	repairValue = FMath::Clamp(repairValue, 0.0f, 500.0f);
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
	if (!USafeENGINE::IsValid(_tempInstance) || !USafeENGINE::IsValid(_tempUserState))
		return false;
	FShipData _tempShipData = _tempInstance->GetShipData(sShipID);
	
	FItemData _tempModuleData;
	TArray<FSkill> _skillList;
	FSkillData _tempSkillData;
	TArray<FBonusStat> _tempbonusStats;
	bool isFindSameAttribute = false;

	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("BonusStatType"), true);
	_tempbonusStats.Reserve(EnumPtr->NumEnums());

	sMaxShield = _tempShipData.Shield;
	sRechargeShield = _tempShipData.RechargeShield;
	sDefShield = _tempShipData.DefShield;

	sMaxArmor = _tempShipData.Armor;
	sRepairArmor = _tempShipData.RepairArmor;
	sDefArmor = _tempShipData.DefArmor;

	sMaxHull = _tempShipData.Hull;
	sRepairHull = _tempShipData.RepairHull;
	sDefHull = _tempShipData.DefHull;

	sMaxPower = _tempShipData.Power;
	sRechargePower = _tempShipData.RechargePower;

	sMaxSpeed = _tempShipData.MaxSpeed;
	sMinAcceleration = _tempShipData.MinAcceleration;
	sMaxAcceleration = _tempShipData.MaxAcceleration;

	sMaxRotateRate = _tempShipData.MaxRotateRate;
	sRotateAcceleration = _tempShipData.rotateAcceleraion;
	sRotateDeceleration = _tempShipData.rotateDeceleraion;

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

	_tempbonusStats = _tempShipData.bonusStats;
	for (int index = 0; index < _skillList.Num(); index++) {
		_tempSkillData = _tempInstance->GetSkillData(_skillList[index].skillID);
		_skillList[index].skillLevel = FMath::Clamp(_skillList[index].skillLevel, 0, 5);


		for (FBonusStat& stats : _tempbonusStats) {
			if (stats.bonusStatType == _tempSkillData.BonusType) {
				stats.bonusStat += _tempSkillData.BonusAmountPerLevel * _skillList[index].skillLevel;
				isFindSameAttribute = true;
				break;
			}
		}
		if (isFindSameAttribute == false)
			_tempbonusStats.Emplace(_tempSkillData.BonusType, _tempSkillData.BonusAmountPerLevel * _skillList[index].skillLevel);
	}

	for (FBonusStat& stats : _tempbonusStats) 
		CheckBonusStat(stats.bonusStatType, stats.bonusStat);

	Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIShip();
	UE_LOG(LogClass, Log, TEXT("Ship Totaly Init complete!"));
	return true;
}

void APlayerShip::CheckPassiveTypeModule(BonusStatType type, float value) {
	switch (type) {
	case BonusStatType::BonusMaxShield:				sMaxShield = sMaxShield + value;			break;
	case BonusStatType::BonusRechargeShield:		sRechargeShield = sRechargeShield + value;	break;
	case BonusStatType::BonusDefShield:				sDefShield = sDefShield + value;			break;

	case BonusStatType::BonusMaxArmor:				sMaxArmor = sMaxArmor + value;				break;
	case BonusStatType::BonusRepaireArmor:			sRepairArmor = sRepairArmor + value;		break;
	case BonusStatType::BonusDefArmor:				sDefArmor = sDefArmor + value;				break;

	case BonusStatType::BonusMaxHull:				sMaxHull = sMaxHull + value;				break;
	case BonusStatType::BonusRepaireHull:			sRepairHull = sRepairHull + value;			break;
	case BonusStatType::BonusDefHull:				sDefHull = sDefHull + value;				break;

	case BonusStatType::BonusMaxPower:				sMaxPower = sMaxPower + value;				break;
	case BonusStatType::BonusRechargePower:			sRechargePower = sRechargePower + value;	break;

	case BonusStatType::BonusMobilitySpeed:			sMaxSpeed = sMaxSpeed * (1.0f + value);	break;
	case BonusStatType::BonusMobilityAcceleration:
		sMinAcceleration = sMinAcceleration * (1.0f + value);
		sMaxAcceleration = sMaxAcceleration * (1.0f + value);
		break;
	case BonusStatType::BonusMobilityRotation:		sMaxRotateRate = sMaxRotateRate * (1.0f + value);	break;
	case BonusStatType::BonusMobilityRotateAcceleration:
		sRotateAcceleration = sRotateAcceleration * (1.0f + value);
		sRotateDeceleration = sRotateDeceleration * (1.0f + value);
		break;
	default:
		break;
	}
}

void APlayerShip::CheckBonusStat(BonusStatType type, float value) {
	switch (type) {
	case BonusStatType::BonusMaxShield:
		sMaxShield = FMath::Clamp(sMaxShield * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 10.0f, 1000000.0f);
		break;
	case BonusStatType::BonusRechargeShield:
		sRechargeShield = FMath::Clamp(sRechargeShield * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 500.0f);
		break;
	case BonusStatType::BonusDefShield:
		sDefShield = FMath::Clamp(sDefShield * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 1000.0f);
		break;
	case BonusStatType::BonusMaxArmor:
		sMaxArmor = FMath::Clamp(sMaxArmor * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 10.0f, 1000000.0f);
		break;
	case BonusStatType::BonusRepaireArmor:
		sRepairArmor = FMath::Clamp(sRepairArmor * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 500.0f);
		break;
	case BonusStatType::BonusDefArmor:
		sDefArmor = FMath::Clamp(sDefArmor * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 1000.0f);
		break;
	case BonusStatType::BonusMaxHull:
		sMaxHull = FMath::Clamp(sMaxHull * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 10.0f, 1000000.0f);
		break;
	case BonusStatType::BonusRepaireHull:
		sRepairHull = FMath::Clamp(sRepairHull * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 500.0f);
		break;
	case BonusStatType::BonusDefHull:
		sDefHull = FMath::Clamp(sDefHull * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 1000.0f);
		break;
	case BonusStatType::BonusMaxPower:
		sMaxPower = FMath::Clamp(sMaxPower * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 10.0f, 1000000.0f);
		break;
	case BonusStatType::BonusRechargePower:
		sRechargePower = FMath::Clamp(sRechargePower * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 500.0f);
		break;
	case BonusStatType::BonusMobilitySpeed:
		sMaxSpeed = FMath::Clamp(sMaxSpeed * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 10000.0f);
		break;
	case BonusStatType::BonusMobilityAcceleration:
		sMaxAcceleration = FMath::Clamp(sMaxAcceleration * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 1000.0f);
		sMinAcceleration = FMath::Clamp(sMinAcceleration * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 1000.0f);
		break;
	case BonusStatType::BonusMobilityRotation:
		sMaxRotateRate = FMath::Clamp(sMaxRotateRate * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 90.0f);
		break;
	case BonusStatType::BonusMobilityRotateAcceleration:
		sRotateAcceleration = FMath::Clamp(sRotateAcceleration * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 90.0f);
		sRotateDeceleration = FMath::Clamp(sRotateDeceleration * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 90.0f);
		break;

	case BonusStatType::BonusDroneBaseStats:
		bonusDroneBaseStats = 1.0f + FMath::Clamp(value, 0.0f, 5.0f);
		break;
	case BonusStatType::BonusDroneControl:
		bonusDroneDamage = 1.0f + FMath::Clamp(value, 0.0f, 5.0f);
		break;
	case BonusStatType::BonusDroneDamage:
		bonusDroneRange = 1.0f + FMath::Clamp(value, 0.0f, 5.0f);
		break;
	case BonusStatType::BonusDroneRange:
		bonusDroneSpeed = 1.0f + FMath::Clamp(value, 0.0f, 5.0f);
		break;
	case BonusStatType::BonusDroneSpeed:
		bonusDroneControl = FMath::Clamp(value, 0.0f, 20.0f);
		break;

	default:
		for (int index = 0; index < slotActiveModule.Num(); index++)
			if (slotActiveModule[index].moduleID > 0 && slotActiveModule[index].moduleType == ModuleType::ShieldGenerator)
				slotActiveModule[index].maxActiveModuleFactor = FMath::Clamp(slotActiveModule[index].maxActiveModuleFactor * (1.0f + FMath::Clamp(value, 0.0f, 5.0f)), 0.0f, 1000.0f);
		for (int index = 0; index < slotTargetModule.Num(); index++) {
			if (slotTargetModule[index].moduleID > 0) {
				switch (slotTargetModule[index].moduleType) {
				case ModuleType::Beam:
					if (type == BonusStatType::BonusBeamDamage)
						slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusBeamPower)
						slotTargetModule[index].maxUsagePower *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusBeamCoolTime)
						slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusBeamAccuracy)
						slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusBeamRange)
						slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					slotTargetModule[index].ammoLifeSpanBonus = FMath::Clamp(slotTargetModule[index].ammoLifeSpanBonus, 0.0f, slotTargetModule[index].maxCooltime);
					break;
				case ModuleType::Cannon:
					if (type == BonusStatType::BonusCannonDamage)
						slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusCannonCoolTime)
						slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusCannonAccuracy)
						slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusCannonLifeTime)
						slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusCannonLaunchVelocity)
						slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					break;
				case ModuleType::Railgun:
					if (type == BonusStatType::BonusRailGunDamage)
						slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusRailGunPower)
						slotTargetModule[index].maxUsagePower *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusRailGunCoolTime)
						slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusRailGunAccuracy)
						slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusRailGunLifeTime)
						slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusRailGunLaunchVelocity)
						slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					break;
				case ModuleType::MissileLauncher:
					if (type == BonusStatType::BonusMissileDamage)
						slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusMissileCoolTime)
						slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusMissileAccuracy)
						slotTargetModule[index].accaucy *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusMissileLifeTime)
						slotTargetModule[index].ammoLifeSpanBonus *= FMath::Clamp(1.0f + value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusMissileLaunchVelocity)
						slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					break;
				case ModuleType::MinerLaser:
					if (type == BonusStatType::BonusMiningAmount)
						slotTargetModule[index].damageMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					else if (type == BonusStatType::BonusMiningPower)
						slotTargetModule[index].maxUsagePower *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusMiningCoolTime)
						slotTargetModule[index].maxCooltime *= FMath::Clamp(1.0f - value, 0.0f, 5.0f);
					else if (type == BonusStatType::BonusMiningRange)
						slotTargetModule[index].launchSpeedMultiple *= FMath::Clamp(1.0f + value, 1.0f, 5.0f);
					break;
				default:
					break;
				}
			}
		}
		break;
	}
}

bool APlayerShip::LoadFromSave(USaveLoader* loader) {

	if (sIsInited == false) {
		sCurrentShield = FMath::Clamp(loader->shield, 0.0f, sMaxShield);
		sCurrentArmor = FMath::Clamp(loader->armor, 0.0f, sMaxArmor);
		sCurrentHull = FMath::Clamp(loader->hull, 0.0f, sMaxHull);
		sCurrentPower = FMath::Clamp(loader->power, 0.0f, sMaxPower);

		USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
		if (!USafeENGINE::IsValid(_tempInstance))
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
			else slotTargetModule[index] = FTargetModule();
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

bool APlayerShip::EquipModule(int moduleID) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempModuleData = _tempInstance->GetItemData(moduleID);
	if (USafeENGINE::IsValid(_tempInstance))
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

bool APlayerShip::UnEquipModule(ItemType moduleItemType, int slotNumber) {
	if (behaviorState != BehaviorState::Docked) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][EquipModule] Ship is not docked, so can't unequip module."));
		return false;
	}

	AUserState* userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
	switch (moduleItemType)
	{
	case ItemType::TargetModule:
		if (slotTargetModule.Num() < slotNumber || slotTargetModule[slotNumber].moduleID <= 0)
			return false;

		switch (slotTargetModule[slotNumber].moduleType) {
		case ModuleType::Beam:
		case ModuleType::TractorBeam:
		case ModuleType::MinerLaser:
			slotTargetModule[slotNumber] = FTargetModule();
			break;
		case ModuleType::Cannon:
		case ModuleType::Railgun:
		case ModuleType::MissileLauncher:
			if (userState->AddPlayerCargo(slotTargetModule[slotNumber].ammo))
				slotTargetModule[slotNumber] = FTargetModule();
			else return false;
			break;
		default:
			break;
		}
		break;
	case ItemType::ActiveModule:
		if (slotActiveModule.Num() < slotNumber || slotActiveModule[slotNumber].moduleID <= 0)
			return false;
		slotActiveModule[slotNumber] = FActiveModule();
		break;
	case ItemType::PassiveModule:
		if (slotPassiveModule.Num() < slotNumber || slotPassiveModule[slotNumber] <= 0)
			return false;
		slotPassiveModule[slotNumber] = 0;
		break;
	case ItemType::SystemModule:
		if (slotSystemModule.Num() < slotNumber || slotSystemModule[slotNumber] <= 0)
			return false;
		slotSystemModule[slotNumber] = 0;
		break;
	default:
		break;
	}
	TotalStatsUpdate();
	return true;
}

void APlayerShip::GetModule(ItemType moduleType, TArray<int>& moduleList) {
	switch (moduleType) {
	case ItemType::TargetModule:
		moduleList.Init(0, slotTargetModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) 
			moduleList[index] = slotTargetModule[index].moduleID;
		break;
	case ItemType::ActiveModule:
		moduleList.Init(0, slotActiveModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) 
			moduleList[index] = slotActiveModule[index].moduleID;
		break;
	case ItemType::PassiveModule:
		moduleList.Init(0, slotPassiveModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) 
			moduleList[index] = slotPassiveModule[index];
		break;
	case ItemType::SystemModule:
		moduleList.Init(0, slotSystemModule.Num());
		for (int index = 0; index < moduleList.Num(); index++) 
			moduleList[index] = slotSystemModule[index];
		break;
	default:
		break;
	}
}

void APlayerShip::GetModuleActivate(ItemType moduleType, TArray<float>& moduleActivate) {

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

void APlayerShip::GetTargetModuleAmmo(TArray<FItem>& targetModuleAmmo) {

	targetModuleAmmo.Init(0, slotTargetModule.Num());
	for (int index = 0; index < targetModuleAmmo.Num(); index++) 
		targetModuleAmmo[index] = slotTargetModule[index].ammo;
}

bool APlayerShip::SetCameraMode() {
	return false;
}

void APlayerShip::ControlCamRotateX(float factorX) {
	playerViewpointArm->AddRelativeRotation(FRotator(0.0f, factorX, 0.0f) * GetWorld()->DeltaTimeSeconds * 5.0f);
	playerViewpointArm->SetWorldRotation(FRotator(FMath::ClampAngle(playerViewpointArm->GetComponentRotation().Pitch, -75.0f, -10.0f), playerViewpointArm->GetComponentRotation().Yaw, 0.0f));
}

void APlayerShip::ControlCamRotateY(float factorY) {
	playerViewpointArm->AddRelativeRotation(FRotator(-factorY, 0.0f, 0.0f) * GetWorld()->DeltaTimeSeconds * 5.0f);
	playerViewpointArm->SetWorldRotation(FRotator(FMath::ClampAngle(playerViewpointArm->GetComponentRotation().Pitch, -75.0f, -10.0f), playerViewpointArm->GetComponentRotation().Yaw, 0.0f));
}

void APlayerShip::ControlCamDistance(float value) {
	SmoothZoomRemain = value * 50.0f;
}

void APlayerShip::ControlViewPointX(float value) {
	playerViewpointArm->AddWorldOffset(FVector(0.0f, value, 0.0f));
}

void APlayerShip::ControlViewPointY(float value) {
	playerViewpointArm->AddWorldOffset(FVector(-value, 0.0f, 0.0f));
}

void APlayerShip::ControlViewPointOrigin() {
	playerViewpointArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

void APlayerShip::SetTargetSpeed(float value) {
	targetSpeed = FMath::Clamp(value, 0.0f, 1.0f);
}

void APlayerShip::SetAcceleration(float value) {
	accelerationFactor = FMath::Clamp(value, 0.0f, 1.0f);
}

void APlayerShip::SetRotateRate(float value) {
	rotateRateFactor = FMath::Clamp(value, 0.0f, 1.0f);
}

/*
* 타게팅 모듈의 동작을 제어. 플레이어 전용.
* @param slotIndex - 타게팅 모듈 슬롯 번호.
* @return 처리 후의 모듈 동작 상태.
*/
bool APlayerShip::ToggleTargetModule(int slotIndex, ASpaceObject* target) {

	//타게팅 모듈에 한해서만( < ModuleType::ShieldGenerator ) 함수 처리
	if (slotIndex < slotTargetModule.Num() && slotIndex < targetingObject.Num() && slotTargetModule[slotIndex].moduleType < ModuleType::ShieldGenerator) {
		//현재 모듈이 활성화되어 있을 경우 -> 작동 중지 예약, 현재 켜져있는 상태임을 리턴
		if (slotTargetModule[slotIndex].moduleState != ModuleState::NotActivate) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : Request Off."), slotIndex);
			slotTargetModule[slotIndex].isBookedForOff = true;
			return true;
		}

		else {
			if (!USafeENGINE::IsValid(target) || target == this) {
				UE_LOG(LogClass, Log, TEXT("[Warning][PlayerShip][CommandToggleTargetModule] Can't find target"));
				return false;
			}

			//현재 꺼져있는 상태
			switch (slotTargetModule[slotIndex].moduleType) {
			case ModuleType::Beam:
			case ModuleType::TractorBeam:
			case ModuleType::MinerLaser:
				//모듈이 빔 계열인 경우
				if (!USafeENGINE::IsValid(target))
					return false;
				slotTargetModule[slotIndex].moduleState = ModuleState::Activate;
				slotTargetModule[slotIndex].isBookedForOff = false;
				slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
				targetingObject[slotIndex] = target;
				UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : On. target is %s."), slotIndex, *target->GetName());
				return true;
				break;
			case ModuleType::Cannon:
			case ModuleType::Railgun:
			case ModuleType::MissileLauncher:
				//모듈이 실탄 계열인 경우, 모듈을 활성화하기 이전에 ammo 상태를 확인
				if (slotTargetModule[slotIndex].ammo.itemAmount < 1) {
					AUserState* userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
					TArray<FItem> _tempItemSlot;
					int _findSlot;

					userState->GetUserDataItem(_tempItemSlot);
					_findSlot = USafeENGINE::FindItemSlot(_tempItemSlot, slotTargetModule[slotIndex].ammo);

					//ammo를 카고 리스트에서 찾았음.
					if (_findSlot > -1) {
						UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - reload : On."), slotIndex);
						slotTargetModule[slotIndex].moduleState = ModuleState::ReloadAmmo;
						targetingObject[slotIndex] = target;
						slotTargetModule[slotIndex].remainCooltime = FMath::Max(1.0f, slotTargetModule[slotIndex].maxCooltime);
					} else
						UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Target Module %d - Toggle : Not On. Ammo not enoght."), slotIndex);
				} else if (USafeENGINE::IsValid(target)) {
					slotTargetModule[slotIndex].moduleState = ModuleState::Activate;
					targetingObject[slotIndex] = target;

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
void APlayerShip::SettingAmmo(int selectedAmmoID) {

	AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
	TArray<FItem> _itemList;
	int _findSlot;

	if (!USafeENGINE::IsValid(_userState))
		return;

	_userState->GetUserDataItem(_itemList);
	_findSlot = USafeENGINE::FindItemSlot(_itemList, FItem(selectedAmmoID, 0));
	if (_findSlot < 0)
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
bool APlayerShip::ToggleActiveModule(int slotIndex) {
	if (slotIndex < slotActiveModule.Num() && slotActiveModule[slotIndex].moduleType > ModuleType::HullRepairLaser && slotActiveModule[slotIndex].moduleType < ModuleType::PassiveModule) {

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
#pragma endregion

#pragma region Interface Implementing : ICommandable
void APlayerShip::CommandStop() {
	if (CheckCanBehavior() == true) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandStop] Receive Command Stop!"));
		behaviorState = BehaviorState::Idle;
		setedTargetSpeed = 0.0f;
		targetRotateRateFactor = 0.0f;
		bIsStraightMove = false;
	}
}

bool APlayerShip::CommandMoveToPosition(FVector position) {
	if (CheckCanBehavior() == true) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandMove] Receive Command Move! : %.2f, %.2f, %.2f"), position.X, position.Y, position.Z);

		moveTargetVector = position;
		targetObject = nullptr;
		bIsStraightMove = true;
		RequestPathUpdate();
		behaviorState = BehaviorState::Move;
		
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OnUIMove(position, FColor::White, 5.0f, lengthToLongAsix);
		return true; 
	}
	else return false;
}

bool APlayerShip::CommandMoveToTarget(ASpaceObject* target) {

	if (CheckCanBehavior() == true) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandMove] Receive Command Move! : %s"), *target->GetName());
		targetObject = target;
		bIsStraightMove = true;
		RequestPathUpdate();
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
	
	if (CheckCanBehavior() == true) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandJump] Receive Command Jump! : %s"), *target->GetDestinationName());
		Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->Jump(target->GetDestinationName());
		return true;
	}
	else return false;
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
		if (target->RequestedDock(Faction::Player)) {
			targetStructure = target;
			targetObject = Cast<ASpaceObject>(target.GetObjectRef());

			if (USafeENGINE::CheckDistacneConsiderSize(this, targetObject) < 500.0f) {
				targetObject = nullptr;
				moveTargetVector = Cast<AActor>(targetStructure.GetObjectRef())->GetActorForwardVector() * 1000.0f + GetActorLocation();
				behaviorState = BehaviorState::Docked;
				Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->SetDockedStructure(targetStructure);
				Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OnUIStationButton();
			} else {
				RequestPathUpdate();
				behaviorState = BehaviorState::Docking;
				UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandDock] Docking to Station Accepted. Start Sequence Dock."));
			}
			bIsStraightMove = true;
			return true;
		}
		else {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandDock] Docking to Station Rejected. Order Cancel."));
			return false;
		}
	}
	else return false;
}

bool APlayerShip::CommandUndock() {
	if (behaviorState == BehaviorState::Docked) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandUndock] Receive Command Undock!"));
		Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->SetDockedStructure(nullptr);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OffUIStationButton();
		bIsStraightMove = true;
		behaviorState = BehaviorState::Idle;	
		return true;
	}
	else return false;
}

bool APlayerShip::CommandLaunch(TArray<int> baySlot) {
	if (CheckCanBehavior() == true) { 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandLaunch] Receive Command Launch! : %d"), baySlot.Num());
		return true;
	}
	else return false;
}
#pragma endregion

#pragma region Functions
bool APlayerShip::MoveDistanceCheck() {
	if (bIsStraightMove) {
		targetVector = moveTargetVector;
		realMoveFactor = targetVector - GetActorLocation();
		DrawDebugCircle(GetWorld(), targetVector, lengthToLongAsix, 40, FColor::Yellow, false, 0.05f, 0, 1.0f, FVector::ForwardVector, FVector::RightVector);
		DrawDebugLine(GetWorld(), targetVector, GetActorLocation(), FColor::Yellow, false, 0.1f);
	}
	else {
		if (USafeENGINE::IsValid(targetObject))
			moveTargetVector = USafeENGINE::CheckLocationMovetoTarget(this, targetObject, 500.0f);
 
		remainDistance = FVector::Dist(moveTargetVector, GetActorLocation());

		moveTargetVector -= GetActorLocation();
		moveTargetVector.Normalize();
		moveTargetVector = GetActorLocation() + moveTargetVector * remainDistance;
		DrawDebugCircle(GetWorld(), moveTargetVector, lengthToLongAsix, 40, FColor::Yellow, false, 0.05f, 0, 1.0f, FVector::ForwardVector, FVector::RightVector);

		currentClosedPathIndex = FMath::Clamp(currentClosedPathIndex, 0, FMath::Max(wayPoint.Num() - 1, 0));
		if (wayPoint.Num() > currentClosedPathIndex)
			targetVector = wayPoint[currentClosedPathIndex];
		else return false;

		for (int index = currentClosedPathIndex; index < wayPoint.Num(); index++) {
			DrawDebugPoint(GetWorld(), wayPoint[index], 5, FColor::Yellow, false, 0.1f);
			if (wayPoint.Num() > index + 1)
				DrawDebugLine(GetWorld(), wayPoint[index], wayPoint[index + 1], FColor::Yellow, false, 0.1f);
		}
	}
	targetVector.Z = 0.0f;

	nextPointDistance = FVector::Dist(targetVector, GetActorLocation());
	targetRotate = realMoveFactor.Rotation() - GetActorRotation();

	//checks distance and Angle For start Acceleration.
	if (nextPointDistance > (FMath::Pow(currentSpeed, 2) / FMath::Clamp(sMinAcceleration * accelerationFactor * 2.0f, 1.0f, 9999.0f) + 5.0f)) {
		if (FMath::Abs(targetRotate.Yaw) < sStartAccelAngle)
			setedTargetSpeed = targetSpeed * sMaxSpeed * (1.0f + moduleStatEngine);
		else setedTargetSpeed = 0.0f;
	}
	else setedTargetSpeed = 0.0f;

	//arrive to Destination. use upper of Nyquist Rate for high precision.
	if (!bIsStraightMove && nextPointDistance <= FMath::Max(5.0f, currentSpeed * tempDeltaTime * 20.0f)) 
		currentClosedPathIndex = FMath::Clamp(currentClosedPathIndex + 1, 0, wayPoint.Num() - 1);

	if (remainDistance < currentSpeed * tempDeltaTime * 50.0f) {
		setedTargetSpeed = 0.0f;
		currentClosedPathIndex = 0;
		bIsStraightMove = false;
		return true;
	}
	//arrive to Destination not yet.
	return false;
}

void APlayerShip::RotateCheck() {

	if (bIsStraightMove) {
		targetVector = moveTargetVector;
	}
	else {
		currentClosedPathIndex = FMath::Clamp(currentClosedPathIndex, 0, wayPoint.Num() - 1);
		if(wayPoint.IsValidIndex(currentClosedPathIndex))
			targetVector = wayPoint[currentClosedPathIndex];
		else return;
	}
	targetVector.Z = 0;

	realMoveFactor = targetVector - GetActorLocation();
	targetRotate = realMoveFactor.Rotation() - GetActorRotation();

	nextPointOuter = FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(GetActorForwardVector(), realMoveFactor));

	if (nextPointOuter > 0.01f) {
		if (FMath::Abs(targetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(sRotateDeceleration * rotateRateFactor * 2.0f, 1.0f, 9999.0f)))
			targetRotateRateFactor = sMaxRotateRate * (1.0f + moduleStatThruster) * rotateRateFactor;
		else
			targetRotateRateFactor = 0.0f;
	}
	else if (nextPointOuter < -0.01f) {
		if (FMath::Abs(targetRotate.Yaw) > FMath::Abs(FMath::Pow(realRotateRateFactor, 2) / FMath::Clamp(sRotateDeceleration * rotateRateFactor * 2.0f, 1.0f, 9999.0f)))
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
					if (USafeENGINE::IsValid(targetingObject[index]) && targetingObject[index] != this && targetingObject[index]->IsA(ASpaceObject::StaticClass())) {
						//목표 지점 및 방향 계산
						FVector _targetedLocation = targetingObject[index]->GetActorLocation();
						FVector _targetedDirect = _targetedLocation - GetActorLocation();
						_targetedDirect.Normalize();
						FRotator _targetedRotation = _targetedDirect.Rotation();
						
						_targetedLocation = GetActorLocation() + _targetedDirect * lengthToLongAsix;
							//+ _targetedRotation.RotateVector(FVector::RightVector) * FMath::FRandRange(-lengthToLongAsix * 0.35f, lengthToLongAsix * 0.35f);
						FTransform _spawnedTransform = FTransform(_targetedRotation, _targetedLocation);

						//빔계열 모듈의 경우
						if (slotTargetModule[index].moduleType == ModuleType::Beam ||
							slotTargetModule[index].moduleType == ModuleType::MinerLaser || 
							slotTargetModule[index].moduleType == ModuleType::TractorBeam ) {

							ABeam* _beam = Cast<ABeam>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ABeam::StaticClass()
								, _spawnedTransform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
							if (!USafeENGINE::IsValid(_beam))
								return;
							UGameplayStatics::FinishSpawningActor(_beam, _spawnedTransform);

							_beam->SetBeamProperty(this, targetingObject[index], slotTargetModule[index].launchSpeedMultiple,
								slotTargetModule[index].moduleType, slotTargetModule[index].damageMultiple, slotTargetModule[index].ammoLifeSpanBonus);
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
								if (  !USafeENGINE::IsValid(_projectile))
									return;
								UGameplayStatics::FinishSpawningActor(_projectile, _spawnedTransform);

								switch (slotTargetModule[index].moduleType) {
								case ModuleType::Cannon:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, this,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, slotTargetModule[index].ammoLifeSpanBonus);
									break;
								case ModuleType::Railgun:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, this,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, slotTargetModule[index].ammoLifeSpanBonus);
									break;
								case ModuleType::MissileLauncher:
									_projectile->SetProjectileProperty(slotTargetModule[index].ammo.itemID, this,
										slotTargetModule[index].damageMultiple, slotTargetModule[index].launchSpeedMultiple, slotTargetModule[index].ammoLifeSpanBonus, targetingObject[index]);
									break;
								}
							}
						}
					}
					else {
						UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][Tick] Target Module %d - Off."), index);
						slotTargetModule[index].moduleState = ModuleState::NotActivate;
						targetingObject[index] = nullptr;
						slotTargetModule[index].isBookedForOff = false;
					}
					break;
				case ModuleState::ReloadAmmo:
					if (slotTargetModule[index].moduleType == ModuleType::Cannon ||
						slotTargetModule[index].moduleType == ModuleType::Railgun ||
						slotTargetModule[index].moduleType == ModuleType::MissileLauncher) {

						AUserState* userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
						TArray<FItem> _tempItemSlot;
						int _reloadedAmount;
						int _findSlot;

						userState->GetUserDataItem(_tempItemSlot);
						_findSlot = USafeENGINE::FindItemSlot(_tempItemSlot, slotTargetModule[index].ammo);
						
						//ammo를 찾지 못하였음. 재장전 및 모듈 동작 실시하지 않음.
						if (slotTargetModule[index].ammo.itemID < 0 || _findSlot < 0)
							slotTargetModule[index].moduleState = ModuleState::NotActivate;
						//ammo를 찾음. 재장전 실시. 모듈 동작은 재장전이 완료되면 자동으로 시작.
						else {
							_reloadedAmount = FMath::Min(_tempItemSlot[_findSlot].itemAmount, slotTargetModule[index].ammoCapacity - slotTargetModule[index].ammo.itemAmount);
							if (userState->DropPlayerCargo(FItem(_tempItemSlot[_findSlot].itemID, _reloadedAmount))) {
								UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][CommandToggleTargetModule] Reload Start!"));
								slotTargetModule[index].ammo.itemAmount = _reloadedAmount;
								if (USafeENGINE::IsValid(targetingObject[index])) {
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
					targetingObject[index] = nullptr;
					slotTargetModule[index].isBookedForOff = false;
				}
			}
		}
		//모듈이 꺼져있을 경우 power 소모량 감소
		else
			slotTargetModule[index].currentUsagePower = FMath::Clamp(slotTargetModule[index].currentUsagePower - slotTargetModule[index].decrementUsagePower * 0.5f,
				0.0f, slotTargetModule[index].maxUsagePower);
		moduleConsumptPower += slotTargetModule[index].currentUsagePower;
	}
	for (int index = 0; index < slotActiveModule.Num(); index++) {
		if (sCurrentPower < 5.0f)
			slotActiveModule[index].moduleState = ModuleState::NotActivate;
		if (slotActiveModule[index].moduleState == ModuleState::Activate) {
			slotActiveModule[index].currentUsagePower = FMath::Clamp(slotActiveModule[index].currentUsagePower + slotActiveModule[index].incrementUsagePower * 0.5f,
				0.0f, slotActiveModule[index].maxUsagePower);
			slotActiveModule[index].currentActiveModuleFactor = FMath::Clamp(slotActiveModule[index].currentActiveModuleFactor + slotActiveModule[index].incrementActiveModuleFactor * 0.5f,
				0.0f, slotActiveModule[index].maxActiveModuleFactor);
		}
		else {
			slotActiveModule[index].currentUsagePower = FMath::Clamp(slotActiveModule[index].currentUsagePower - slotActiveModule[index].decrementUsagePower * 0.5f,
				0.0f, slotActiveModule[index].maxUsagePower);
			slotActiveModule[index].currentActiveModuleFactor = FMath::Clamp(slotActiveModule[index].currentActiveModuleFactor - slotActiveModule[index].decrementActiveModuleFactor * 0.5f,
				0.0f, slotActiveModule[index].maxActiveModuleFactor);
		}
		moduleConsumptPower += slotActiveModule[index].currentUsagePower;
		switch (slotActiveModule[index].moduleType) {
		case ModuleType::ShieldGenerator:	moduleStatShieldRegen += slotActiveModule[index].currentActiveModuleFactor;		break;
		case ModuleType::ArmorRepairer:		moduleStatArmorRepair += slotActiveModule[index].currentActiveModuleFactor;		break;
		case ModuleType::HullRepairer:		moduleStatHullRepair += slotActiveModule[index].currentActiveModuleFactor;		break;
		case ModuleType::EngineController:	moduleStatEngine += slotActiveModule[index].currentActiveModuleFactor;			break;
		case ModuleType::Accelerator:		moduleStatAcceleration += slotActiveModule[index].currentActiveModuleFactor;	break;
		case ModuleType::SteeringController:moduleStatThruster += slotActiveModule[index].currentActiveModuleFactor;		break;
		default:	break;
		}
	}
}

bool APlayerShip::CheckCanBehavior() {
	switch (behaviorState)
	{
	case BehaviorState::Docked:
	case BehaviorState::Warping:
		return false;
	default:
		return true;
	}
}

float APlayerShip::CalculateCompute() {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempModuleData;
	float _result = 0;
	if (!USafeENGINE::IsValid(_tempInstance))
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

float APlayerShip::CalculatePowerGrid() {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempModuleData;
	float _result = 0;
	if (!USafeENGINE::IsValid(_tempInstance))
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

#pragma region Path Finder
/*
* Check 1 : 전방 장애물 검사. 길이 : 오브젝트의 장축 * 2.0f + 1000.0f. 
* Check 2 : 목표 지점까지의 장애물 검사. 거리 : 목표 지점까지
* Check 1 또는 Check 2가 true일 때, RequestPathUpdate()를 호출하여 경로 재탐색.
*/
void APlayerShip::CheckPath() {
	FVector _forTargetDirectionVector = moveTargetVector - GetActorLocation();
	_forTargetDirectionVector.Normalize();

	if (!CheckCanBehavior() || behaviorState == BehaviorState::Idle)
		return;

	bool bMoveTargetHited = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), GetActorLocation() + _forTargetDirectionVector * (lengthToLongAsix + 10.0f)
		, moveTargetVector, FVector(0.0f, lengthToLongAsix * 0.5f + 10.0f, 50.0f), _forTargetDirectionVector.Rotation()
		, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, frontTraceResult, true);
	bool bFrontHited = UKismetSystemLibrary::BoxTraceMulti(GetWorld(), GetActorLocation() + GetActorForwardVector() * (lengthToLongAsix + 10.0f)
		, GetActorLocation() + GetActorForwardVector() * (lengthToLongAsix * 2.0f + 200.0f), FVector(0.0f, lengthToLongAsix * 0.5f + 10.0f, 50.0f), GetActorRotation()
		, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, frontTraceResult, true);
	
	bIsStraightMove = !bMoveTargetHited;
	if (!(!bMoveTargetHited || !bFrontHited))
		RequestPathUpdate();
}

/*
* 이동 명령 및 전방의 장애물이 확인되었을 때 호출.
* NavMesh 기반 경로 요청.
*/
void APlayerShip::RequestPathUpdate() {
	remainDistance = FVector::Dist(moveTargetVector, GetActorLocation());
	if (USafeENGINE::IsValid(targetObject)) {
		moveTargetVector = targetObject->GetActorLocation();
		remainDistance = FVector::Dist(moveTargetVector, GetActorLocation()) - (lengthToLongAsix + targetObject->GetValue(GetStatType::halfLength));
		moveTargetVector.Normalize();
		moveTargetVector *= remainDistance;
	}

	waypointData = UNavigationSystem::GetCurrent(GetWorld())->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), moveTargetVector);
	wayPoint = waypointData->PathPoints;
	if (!waypointData)
		return;
	bIsStraightMove = false;
	currentClosedPathIndex = 0;
}
#pragma endregion
