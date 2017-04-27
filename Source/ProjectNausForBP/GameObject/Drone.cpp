// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ProjectNausForBP.h"
#include "Drone.h"


ADrone::ADrone() {
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

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AIControllerClass = ASpaceAIController::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0f;
	droneID = -1;
}

#pragma region Event Calls
void ADrone::BeginPlay() {
	Super::BeginPlay();

	UE_LOG(LogClass, Log, TEXT("[Info][Drone][Spawn] Spawn Finish!"));
}

void ADrone::Tick(float DeltaSeconds) {
	Super::BeginPlay();

}

float ADrone::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
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

	if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) > 0.95f) {
		_remainDamage *= 2.0f;
		_isCritical = true;
	}
	else if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) < -0.95f) {
		_remainDamage *= 3.0f;
		_isCritical = true;
	}

	//remainDamage = sDefShield.GetValue();
	if (currentShield > _remainDamage) {
		_effectShieldDamage = _remainDamage;
		currentShield -= _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectShieldDamage = currentShield;
		_remainDamage -= _effectShieldDamage;
		currentShield = 0.0f;
	}

	if (currentArmor > _remainDamage) {
		_effectArmorDamage = _remainDamage;
		currentArmor -= _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectArmorDamage = currentArmor;
		_remainDamage -= _effectArmorDamage;
		currentArmor = 0.0f;
	}

	if (currentHull > _remainDamage) {
		_effectHullDamage = _remainDamage;
		currentHull -= _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectHullDamage = currentHull;
		currentHull = 0.0f;
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Drone][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage,
		_isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void ADrone::BeginDestroy() {

	if (GetWorld() && UGameplayStatics::GetGameState(GetWorld()) && UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass())) {
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(true);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->RemoveFromObjectList(this);
	}
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int ADrone::GetObjectID() const {
	return droneID;
}

ObjectType ADrone::GetObjectType() const {
	return ObjectType::Ship;
}

Faction ADrone::GetFaction() const {
	return faction;
}

void ADrone::SetFaction(const Faction setFaction) {
	faction = setFaction;
	return;
}

BehaviorState ADrone::GetBehaviorState() const {
	return behaviorState;
}

bool ADrone::InitObject(const int objectId) {
	/*
	if (objectId < 0)
	return false;

	USafeENGINE* tmpInstance = Cast<USafeENGINE>(GetGameInstance());
	if (tmpInstance == nullptr)
	return false;

	lengthToLongAsix = tempData.lengthToLongAsix;

	FShipData tempData = tmpInstance->GetShipData(objectId);
	if (&tempData == nullptr)
	return false;

	if (sShipID.GetValue() != objectId) {
	sShipID.SetValue(objectId);
	objectName = ...?
	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *tempData.MeshPath.ToString()));
	objectMesh->SetStaticMesh(newMesh);

	targetModuleList.Empty();
	for (int index = 0; index < tempData.SlotTarget; index++)
	targetModuleList.Add(NewObject<UTargetModule>(this, FName::FName("TargetModuleComponent", index)));

	activeModuleList.Empty();
	for (int index = 0; index < tempData.SlotActive; index++)
	activeModuleList.Add(NewObject<UActiveModule>(this, FName::FName("ActiveModuleComponent", index)));

	passiveModuleList.Init(0, tempData.SlotPassive);
	systemModuleList.Init(0, tempData.SlotSystem);

	UE_LOG(LogClass, Log, TEXT("Ship Init complete - Ship ID : %d, targetModuleList : %d, activeModuleList : %d, passiveModuleList : %d, systemModuleList : %d"),
	sShipID.GetValue(), targetModuleList.Num(), activeModuleList.Num(), passiveModuleList.Num(), systemModuleList.Num());
	}

	sMaxShield = tempData.Shield;
	sRechargeShield = tempData.RechargeShield;
	sDefShield = tempData.DefShield;

	sMaxArmor = tempData.Armor;
	sRepairArmor = tempData.RepairArmor;
	sDefArmor = tempData.DefArmor;

	sMaxHull = tempData.Hull;
	sRepairHull = tempData.RepairHull;
	sDefHull = tempData.DefHull;

	sMaxPower = tempData.Power;
	sRechargePower = tempData.RechargePower;

	sMaxSpeed = tempData.MaxSpeed;
	sMinAcceleration = tempData.MinAcceleration;
	sMaxAcceleration = tempData.MaxAcceleration;
	sMaxRotateRate = tempData.MaxRotateRate;
	*/
	return true;
}

bool ADrone::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {

	return false;
}

float ADrone::GetValue(const GetStatType statType) const {
	float _value;

	switch (statType) {
	case GetStatType::halfLength:
		_value = lengthToLongAsix * 0.5f;
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
	case GetStatType::repaireHull:
		_value = currentHull;
		break;
	case GetStatType::defHull:
		_value = defHull;
		break;
	default:
		_value = 0.0f;
		break;
	}
	return _value = 0.0f;
}

void ADrone::GetRepaired(const GetStatType statType, float repairValue) {
	
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
void ADrone::CommandStop() {

}

bool ADrone::CommandMoveToPosition(FVector position) {
	return false;
}

bool ADrone::CommandMoveToTarget(ASpaceObject* target) {
	return false;
}

bool ADrone::CommandAttack(ASpaceObject* target) {
	return false;
}

bool ADrone::CommandMining(AResource* target) {
	return false;
}

bool ADrone::CommandRepair(ASpaceObject* target) {
	return false;
}

bool ADrone::CommandJump(TScriptInterface<IStructureable> target) {
	return false;
}

bool ADrone::CommandWarp(FVector location) {
	return false;
}

bool ADrone::CommandDock(TScriptInterface<IStructureable> target) {
	return false;
}

bool ADrone::CommandUndock() {
	return false;
}

bool ADrone::CommandLaunch(const TArray<int>& BaySlot) {
	return false;
}
#pragma endregion
