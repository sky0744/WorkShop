// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Resource.h"

AResource::AResource()
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

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0f;
	resourceID = -1;
}

#pragma region Event Calls
void AResource::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogClass, Log, TEXT("[Info][Resource][Spawn] Spawn Finish!"));
}

void AResource::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

float AResource::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {

	float remainDamage = DamageAmount * FMath::FRandRange(0.85f, 1.15f);
	float effectDamage = 0.0f;
	
	//remainDamage = sDefShield.GetValue();
	if (currentDurability > remainDamage) {
		effectDamage = remainDamage;
		currentDurability -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectDamage = currentDurability;
		currentDurability = 0.0f;
		Destroy();
	}
	UE_LOG(LogClass, Log, TEXT("[Info][Resource][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : %.0f"), *this->GetName(), *DamageEvent.DamageTypeClass->GetName(), remainDamage, *DamageCauser->GetName(), effectDamage);

	return effectDamage;
}

void AResource::BeginDestroy() {

	if (GetWorld() && UGameplayStatics::GetGameState(GetWorld()) && UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass())) {
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(true);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->RemoveFromObjectList(this);
	}
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int AResource::GetObjectID() const {
	return resourceID;
}

ObjectType AResource::GetObjectType() const {
	return ObjectType::Resource;
}

Faction AResource::GetFaction() const {
	return Faction::Neutral;
}

void AResource::SetFaction(const Faction setFaction) {
	return;
}

BehaviorState AResource::GetBehaviorState() const {
	return BehaviorState::Idle;
}

bool AResource::InitObject(const int objectId) {
	if (objectId < 0)
		return false;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FResourceData _tempResourceData = _tempInstance->GetResourceData(objectId);
	bool _isRich = false;
	if (FMath::FRandRange(0.0f, 1.0f) > 1.0f - _tempResourceData.RichOreChance) 
		_isRich = true;

	if (resourceID != objectId) {
		resourceID = objectId;
		if(_isRich)
			objectName = FText::Format(NSLOCTEXT("FTextField", "FTextField", "Rich {name}"), _tempResourceData.Name);
		else objectName = _tempResourceData.Name;

		UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempResourceData.MeshPath.ToString()));
		objectMesh->SetStaticMesh(newMesh);
		lengthToLongAsix = _tempResourceData.lengthToLongAsix;

		resourceType = _tempResourceData.Type;
		lengthToLongAsix = _tempResourceData.lengthToLongAsix;

		maxDurability = FMath::FRandRange(_tempResourceData.DurabilityRange.X, _tempResourceData.DurabilityRange.Y);
		currentDurability = maxDurability;
		currentResource = FItem(_tempResourceData.ResourceItemID, FMath::FloorToInt(FMath::FRandRange(_tempResourceData.OreAmountRange.X, _tempResourceData.OreAmountRange.Y)));
		defResource = _tempResourceData.DurabilityDef;
		if (_isRich) {
			currentDurability *= FMath::FRandRange(2.0f, 3.0f);
			currentResource.itemAmount *= FMath::FRandRange(2.0f, 3.0f);
		}
	}
	return true;
}

bool AResource::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {
	return false;
}

float AResource::GetValue(const GetStatType statType) const {
	float _value;

	switch (statType) {
	case GetStatType::halfLength:
		_value = lengthToLongAsix * 0.5f;
		break;
	case GetStatType::maxHull:
		_value = maxDurability;
		break;
	case GetStatType::currentHull:
		_value = currentDurability;
		break;
	case GetStatType::defHull:
		_value = defResource;
		break;
	default:
		_value = 0.0f;
		break;
	}
	return _value;
}
#pragma endregion

#pragma region Resource Functions
void AResource::SetResource(const int resourceId, float durability, int amount) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FResourceData _tempResourceData = _tempInstance->GetResourceData(resourceId);

	currentDurability = FMath::Clamp(durability, 0.0f, maxDurability);
	currentResource = FItem(_tempResourceData.ResourceItemID, FMath::Clamp(amount, 0, FMath::FloorToInt(FMath::Max(_tempResourceData.OreAmountRange.X, _tempResourceData.OreAmountRange.Y))));
}

float AResource::GetResourceAmount() const {
	return currentResource.itemAmount;
}

FItem AResource::CollectResource(float miningPerfomance) {

	int _collectedAmount = FMath::Clamp(FMath::FloorToInt(miningPerfomance * (1.0f - defResource / 1000.0f)), 0, currentResource.itemAmount);
	currentResource.itemAmount -= _collectedAmount;

	if (currentResource.itemAmount <= 0)
		Destroy();
	return FItem(currentResource.itemID, _collectedAmount);
}
#pragma endregion