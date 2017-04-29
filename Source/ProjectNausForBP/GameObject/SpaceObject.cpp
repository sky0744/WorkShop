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
	Faction _dealingFaction;

	if (DamageCauser->IsA(ABeam::StaticClass()))
		_dealingFaction = Cast<ABeam>(DamageCauser)->GetLaunchingFaction();
	else if (DamageCauser->IsA(AProjectiles::StaticClass()))
		_dealingFaction = Cast<AProjectiles>(DamageCauser)->GetLaunchingFaction();
	else
		return 0.0f;

	float _remainDamage = DamageAmount * FMath::FRandRange(0.85f, 1.15f);
	float _effectDamage = 0.0f;

	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	if (USafeENGINE::IsValid(_spaceState))
		_spaceState->ChangeRelationship(faction, _dealingFaction, _remainDamage);

	if (currentDurability > _remainDamage) {
		_effectDamage = _remainDamage;
		currentDurability -= _remainDamage;
		_remainDamage = 0.0f;
	}
	else {
		_effectDamage = currentDurability;
		currentDurability = 0.0f;

		AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
		Peer _peerResult = Peer::Neutral;

		if (USafeENGINE::IsValid(_userState) && USafeENGINE::IsValid(_spaceState)) {
			_peerResult = _spaceState->PeerIdentify(faction, _dealingFaction, true);
			//전략 포인트의 일부 가중치를 팩션 관계도에 반영
			_spaceState->ChangeRelationship(faction, _dealingFaction, true, strategicPoint * FMath::FRandRange(_define_SPtoRelationFactorMIN, _define_SPtoRelationFactorMAX));
			if (_dealingFaction == Faction::Player)
				_userState->ChangeRenown(_peerResult, strategicPoint);
		}
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][SpaceObject][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : %.0f"), 
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectDamage);

	return _effectDamage;
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
int ASpaceObject::GetObjectID() const {
	return 0;
}

void ASpaceObject::GetObjectName(FText& spaceObjectName) const {
	spaceObjectName = objectName;
}

ObjectType ASpaceObject::GetObjectType() const {
	return ObjectType::SpaceObject;
}

Faction ASpaceObject::GetFaction() const {
	return faction;
}

void ASpaceObject::SetFaction(const Faction setFaction) {
	faction = setFaction;
}

BehaviorState ASpaceObject::GetBehaviorState() const {
	return BehaviorState::Idle;
}

bool ASpaceObject::InitObject(const int objectId) {
	return false;
}

bool ASpaceObject::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {
	return false;
}

float ASpaceObject::GetValue(const GetStatType statType) const {
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

void ASpaceObject::GetRepaired(const GetStatType statType, float repairValue) {

	return;
}
#pragma endregion