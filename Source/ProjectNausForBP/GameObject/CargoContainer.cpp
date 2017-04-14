// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "CargoContainer.h"



// Sets default values
ACargoContainer::ACargoContainer()
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
void ACargoContainer::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogClass, Log, TEXT("[Info][CargoContainer][Spawn] Spawn Finish!"));
}

void ACargoContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ACargoContainer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	if (!DamageCauser->IsA(ASpaceObject::StaticClass()))
		return 0.0f;

	float remainDamage = DamageAmount * FMath::FRandRange(0.85f, 1.15f);
	float effectDamage = 0.0f;

	if (currentDurability > remainDamage) {
		effectDamage = remainDamage;
		currentDurability -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectDamage = currentDurability;
		currentDurability = 0.0f;
	}

	UE_LOG(LogClass, Log, TEXT("[Info][CargoContainer][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : %.0f"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), remainDamage, *DamageCauser->GetName(), effectDamage);

	return effectDamage;
}

void ACargoContainer::BeginDestroy() {
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int ACargoContainer::GetObjectID() {
	return 0;
}

ObjectType ACargoContainer::GetObjectType() {
	return ObjectType::SpaceObject;
}

Faction ACargoContainer::GetFaction() {
	return Faction::Neutral;
}

void ACargoContainer::SetFaction(Faction setFaction) {

}

BehaviorState ACargoContainer::GetBehaviorState() {
	return BehaviorState::Idle;
}

bool ACargoContainer::InitObject(int objectId) {

	return false;
}

bool ACargoContainer::LoadBaseObject(float shield, float armor, float hull, float power) {

	return false;
}

float ACargoContainer::GetValue(GetStatType statType) {
	float value;

	switch (statType) {
	case GetStatType::halfLength:
		value = lengthToLongAsix * 0.5f;
		break;
	case GetStatType::maxHull:
		value = maxDurability;
		break;
	case GetStatType::currentHull:
		value = currentDurability;
		break;
	default:
		value = -1.0;
		break;
	}
	return value;
}
#pragma endregion
