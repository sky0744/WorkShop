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
	if (!DamageCauser->IsA(ASpaceObject::StaticClass()))
		return 0.0f;

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
int AResource::GetObjectID() {
	return resourceID;
}

ObjectType AResource::GetObjectType() {
	return ObjectType::Resource;
}

Faction AResource::GetFaction() {
	return Faction::Neutral;
}

void AResource::SetFaction(Faction setFaction) {
	return;
}

BehaviorState AResource::GetBehaviorState() {
	return BehaviorState::Idle;
}

bool AResource::InitObject(int objectId) {
	if (objectId < 0)
		return false;

	USafeENGINE* tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FResourceData tempData = tempInstance->GetResourceData(objectId);

	if (resourceID != objectId) {
		resourceID = objectId;
		objectName = tempData.Name;

		UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *tempData.MeshPath.ToString()));
		objectMesh->SetStaticMesh(newMesh);
		lengthToLongAsix = tempData.lengthToLongAsix;
	}

	resourceType = tempData.Type;
	lengthToLongAsix = tempData.lengthToLongAsix;

	currentDurability = maxDurability = FMath::FRandRange(tempData.DurabilityMin, tempData.DurabilityMax);
	currentResource = FItem(tempData.ResourceID, FMath::FRandRange(tempData.ResourceAmountMin, tempData.ResourceAmountMax));
	defResource = tempData.DurabilityDef;
	if (FMath::FRandRange(0.0f, 1.0f) > 1.0f - tempData.RichOreChance) {
		currentDurability *= 2.0f;
		currentResource.itemAmount *= 2.0f;
		defResource *= 2.0f;
	}

	return true;
}

bool AResource::LoadBaseObject(float shield, float armor, float hull, float power) {
	return false;
}

float AResource::GetValue(GetStatType statType) {
	float value;

	switch (statType) {
	case GetStatType::halfLength:
		value = lengthToLongAsix * 0.5f;
		break;
	default:
		value = -1.0f;
		break;
	}
	return value;
}
#pragma endregion

#pragma region Resource Functions
void AResource::SetResource(float durability, FItem ore) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());

	currentDurability = FMath::Clamp(durability, 0.0f, _tempInstance->GetResourceData(resourceID).DurabilityMax);
	currentResource.itemAmount = FMath::Clamp((float)ore.itemAmount, 0.0f, _tempInstance->GetResourceData(resourceID).ResourceAmountMax);
}

float AResource::GetResourceAmount() {
	return currentResource.itemAmount;
}

float AResource::GetResourceDurability() {
	return currentDurability;
}

float AResource::GetResourceDef() {
	return defResource;
}

FItem AResource::CollectResource(float miningPerfomance) {
	float _collectedAmount = FMath::Clamp(FMath::Max(miningPerfomance * (1000.0f - defResource / 1000.0f), 1.0f), 0.0f, (float)currentResource.itemAmount);
	currentResource.itemAmount -= _collectedAmount;

	if (currentResource.itemAmount <= 0.0f)
		Destroy();

	return FItem(currentResource.itemID, _collectedAmount);
}
#pragma endregion