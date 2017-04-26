// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "SpaceObject.h"

ASpaceObject::ASpaceObject()
{
	objectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
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

	objectID = -1;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

#pragma region Event Calls
void ASpaceObject::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld() && UGameplayStatics::GetGameState(GetWorld()) && UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass())) {
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(false);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->AddToObjectList(this);
	}
	lengthToLongAsix = 0.0f;

	float tempX = FMath::FRandRange(0.001f, 0.01f);
	if (FMath::RandBool())
		tempX *= -1.0f;
	float tempY = FMath::FRandRange(0.001f, 0.01f);
	if (FMath::RandBool())
		tempY *= -1.0f;
	AddActorWorldOffset(FVector(tempX, tempY, 0.0f));
}

void ASpaceObject::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

float ASpaceObject::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {

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
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][SpaceObject][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : %.0f"), 
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), remainDamage, *DamageCauser->GetName(), effectDamage);

	return effectDamage;
}

void ASpaceObject::BeginDestroy() {

	if (GetWorld() && UGameplayStatics::GetGameState(GetWorld()) && UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass())) {
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(true);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->RemoveFromObjectList(this);
	}
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int ASpaceObject::GetObjectID() {
	return 0;
}

void ASpaceObject::GetObjectName(FText& spaceObjectName) {
	spaceObjectName = objectName;
}

ObjectType ASpaceObject::GetObjectType() {
	return ObjectType::SpaceObject;
}

Faction ASpaceObject::GetFaction() {
	return Faction::Neutral;
}

void ASpaceObject::SetFaction(Faction setFaction) {

}


BehaviorState ASpaceObject::GetBehaviorState() {
	return BehaviorState::Idle;
}

bool ASpaceObject::InitObject(int objectId) {

	return false;
}

bool ASpaceObject::LoadBaseObject(float shield, float armor, float hull, float power) {

	return false;
}

float ASpaceObject::GetValue(GetStatType statType) {
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
	default:
		_value = 0.0f;
		break;
	}
	return _value;
}

void ASpaceObject::GetRepaired(GetStatType statType, float repairValue) {
	repairValue = FMath::Clamp(repairValue, 0.0f, 500.0f);
	switch (statType) {
	case GetStatType::currentHull:
		currentDurability = FMath::Clamp(currentDurability + repairValue, 0.0f, maxDurability);
		break;
	default:
		break;
	}
}
#pragma endregion