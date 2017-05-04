// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "CargoContainer.h"


// Sets default values
ACargoContainer::ACargoContainer()
{
	objectMesh->SetCanEverAffectNavigation(true);
	objectMesh->SetEnableGravity(false);
	objectMesh->SetSimulatePhysics(true);
	objectMesh->BodyInstance.LinearDamping = 50.0f;
	objectMesh->BodyInstance.AngularDamping = 500.0f;
	objectMesh->BodyInstance.bLockZTranslation = true;
	objectMesh->BodyInstance.bLockXRotation = true;
	objectMesh->BodyInstance.bLockYRotation = true;
	objectMesh->Mobility = EComponentMobility::Movable;
	RootComponent = objectMesh;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0f;
	cargoContainerID = -1;
}

#pragma region Event Calls
void ACargoContainer::BeginPlay()
{
	Super::BeginPlay();

	containerRotator = FRotator();
	containerRotator.Pitch = FMath::FRandRange(_define_RandomRotateSpeedMIN, _define_RandomRotateSpeedMAX);
	containerRotator.Yaw = FMath::FRandRange(_define_RandomRotateSpeedMIN, _define_RandomRotateSpeedMAX);
	containerRotator.Roll = FMath::FRandRange(_define_RandomRotateSpeedMIN, _define_RandomRotateSpeedMAX);
	UE_LOG(LogClass, Log, TEXT("[Info][CargoContainer][Spawn] Spawn Finish!"));
}

void ACargoContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(containerRotator);
}

float ACargoContainer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {

	float _remainDamage = DamageAmount * FMath::FRandRange(0.85f, 1.15f);
	float _effectDamage = 0.0f;

	if (currentDurability > _remainDamage) {
		_effectDamage = _remainDamage;
		currentDurability -= _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectDamage = currentDurability;
		currentDurability = 0.0f;
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][CargoContainer][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : %.0f"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectDamage);

	return _effectDamage;
}

void ACargoContainer::BeginDestroy() {

	if (GetWorld() && UGameplayStatics::GetGameState(GetWorld()) && UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass())) {
		Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(true);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->RemoveFromObjectList(this);
	}
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int ACargoContainer::GetObjectID() const {
	return 0;
}

ObjectType ACargoContainer::GetObjectType() const {
	return ObjectType::Container;
}

Faction ACargoContainer::GetFaction() const {
	return Faction::Neutral;
}

void ACargoContainer::SetFaction(const Faction setFaction) {
	faction = Faction::Neutral;
}

BehaviorState ACargoContainer::GetBehaviorState() const {
	return BehaviorState::Idle;
}

bool ACargoContainer::InitObject(const int objectId) {

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!USafeENGINE::IsValid(_tempInstance))
		return false;

	FObjectData _tempObjectData = _tempInstance->GetCargoContainerData(objectId);
	objectName = _tempObjectData.Name;
	lengthToLongAsix = _tempObjectData.LengthToLongAsix;
	UStaticMesh* _newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempObjectData.MeshPath.ToString()));
	if (_newMesh)
		objectMesh->SetStaticMesh(_newMesh);

	return false;
}

bool ACargoContainer::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {
	return false;
}

float ACargoContainer::GetValue(const GetStatType statType) const {
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
#pragma endregion

#pragma region Functions
void ACargoContainer::SetCargo(const FItem& items) {

	if (cargo.itemID < 0)
		return;
	cargo = FItem(items);
}

FItem ACargoContainer::GetCargo() {

	FItem _retrunCargo = cargo;
	cargo.itemID = -1;

	return _retrunCargo;
}
#pragma endregion
