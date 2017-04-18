// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Gate.h"

AGate::AGate()
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

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 5.0f;
}

#pragma region Event Calls
void AGate::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogClass, Log, TEXT("[Info][Gate][Spawn] Spawn Finish!"));
}

void AGate::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	currentShield = FMath::Clamp(currentShield + rechargeShield * DeltaTime, 0.0f, maxShield);
	currentArmor = FMath::Clamp(currentArmor + repairArmor * DeltaTime, 0.0f, maxArmor);
	currentHull = FMath::Clamp(currentHull + repairHull * DeltaTime, 0.0f, maxHull);
}

float AGate::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	if (!DamageCauser->IsA(ASpaceObject::StaticClass()))
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
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, this->GetName() + " is Die!");
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Drone][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"), *this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage, _isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void AGate::BeginDestroy() {
	if (structureInfo) {
		structureInfo->isDestroyed = true;
		structureInfo->remainRespawnTime = structureInfo->maxRespawnTime;
	}

	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int AGate::GetObjectID() {
	if (structureInfo != nullptr)
		return structureInfo->structureID;
	else return -1;
}

ObjectType AGate::GetObjectType() {
	return ObjectType::Gate;
}

Faction AGate::GetFaction() {
	if (structureInfo != nullptr)
		return structureInfo->structureFaction;
	else return Faction::Neutral;
}

void AGate::SetFaction(Faction setFaction) {
	return;
}

BehaviorState AGate::GetBehaviorState() {
	return BehaviorState::Idle;
}

bool AGate::InitObject(int objectId) {
	return false;
}

bool AGate::LoadBaseObject(float shield, float armor, float hull, float power) {
	return false;
}

float AGate::GetValue(GetStatType statType) {
	float value;

	switch (statType) {
	case GetStatType::halfLength:
		value = lengthToLongAsix * 0.5f;
		break;
	case GetStatType::maxShield:
		value = maxShield;
		break;
	case GetStatType::rechargeShield:
		value = rechargeShield;
		break;
	case GetStatType::currentShield:
		value = currentShield;
		break;
	case GetStatType::defShield:
		value = defShield;
		break;

	case GetStatType::maxArmor:
		value = maxArmor;
		break;
	case GetStatType::repaireArmor:
		value = repairArmor;
		break;
	case GetStatType::currentArmor:
		value = currentArmor;
		break;
	case GetStatType::defArmor:
		value = defArmor;
		break;

	case GetStatType::maxHull:
		value = maxHull;
		break;
	case GetStatType::repaireHull:
		value = repairHull;
		break;
	case GetStatType::currentHull:
		value = currentHull;
		break;
	case GetStatType::defHull:
		value = defHull;
		break;
	default:
		value = -1;
		break;
	}

	return value;
}

void AGate::GetRepaired(GetStatType statType, float repairValue) {

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

#pragma region Interface Implementation : IStructureable
FString AGate::GetDestinationName() {
	return structureInfo->LinkedSector;
}

StructureType AGate::GetStationType() {
	return StructureType::Gate;
}

bool AGate::RequestedDock(Faction requestFaction) {
	return true;
}
bool AGate::RequestedJump(Faction requestFaction) {
	return true;
}

bool AGate::SetStructureData(FStructureInfo& structureData) {
	if (isInited)
		return false;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FStationData _tempData;
	
	structureInfo = &structureData;
	_tempData = _tempInstance->GetStationData(structureInfo->structureID);

	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempData.MeshPath.ToString()));
	objectMesh->SetStaticMesh(newMesh);
	lengthToLongAsix = _tempData.lengthToLongAsix;

	maxShield = _tempData.Shield;
	currentShield = structureInfo->structureShieldRate * maxShield;
	rechargeShield = _tempData.RechargeShield;
	defShield = _tempData.DefShield;

	maxArmor = _tempData.Armor;
	currentArmor = structureInfo->structureArmorRate * maxArmor;
	repairArmor = _tempData.RepairArmor;
	defArmor = _tempData.DefArmor;

	maxHull = _tempData.Hull;
	currentHull = structureInfo->structureHullRate * maxHull;
	repairHull = _tempData.RepairHull;
	defHull = _tempData.DefHull;

	isInited = true;
	return true;
}

void AGate::GetStructureData(FStructureInfo& structureData) {
	structureData = *structureInfo;
	return;
}
#pragma endregion

#pragma region Functions
FStructureInfo* AGate::GetStructureDataPointer() {
	return structureInfo;
}

void AGate::CheckGateRefreshTime(){

}
void AGate::RefreshGateItem(){

}

#pragma endregion
