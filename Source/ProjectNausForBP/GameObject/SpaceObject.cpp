// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceObject.h"

ASpaceObject::ASpaceObject()
{
	objectRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectRoot"));
	RootComponent = objectRoot;
	objectSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ObjectSprite"));
	objectSprite->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	objectSprite->bAbsoluteRotation = true;
	objectSprite->SetWorldRotation(FRotator(0.0f, 90.0f, -90.0f));
	objectSprite->SetEnableGravity(false);
	objectSprite->SetCollisionProfileName(TEXT("SpaceObject"));
	objectSprite->BodyInstance.DOFMode = EDOFMode::XYPlane;
	objectSprite->Mobility = EComponentMobility::Movable;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0f;

	objectID = -1;
}

#pragma region Event Calls
void ASpaceObject::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld()) {
		if(Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld())))
			Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(false);
		if(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass()) && this != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->AddToObjectList(this);
	}
}

void ASpaceObject::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
	/* 위쪽 방향부터 시계 방향으로 회전시
	if (IsValid(objectFlipBook) && objectFlipBook->GetNumFrames() > 1) {
		objectYaw = GetActorRotation().Yaw;
		objectYaw += (360.0f / objectFlipBook->GetNumFrames()) * 0.5f;
		if (objectYaw < 0.0f)
			objectYaw += 360.0f;
		objectYaw /= (360.0f / objectFlipBook->GetNumFrames());
		objectSprite->SetSprite(objectFlipBook->GetSpriteAtFrame(FMath::TruncToInt(objectYaw)));
	}
	*/
	//현 : 왼쪽 방향부터 반시계 방향
	float _temp = DeltaTime;
	if (IsValid(objectFlipBook) && objectFlipBook->GetNumFrames() > 1) {
		//성능을 위해 캐싱으로 활용
		DeltaTime = 360.0f / objectFlipBook->GetNumFrames();
		objectYaw = -GetActorRotation().Yaw - 90.0f + DeltaTime * 0.5f;
		if (objectYaw < 0.0f)
			objectYaw += 360.0f;
		objectYaw /= DeltaTime;
		GEngine->AddOnScreenDebugMessage(-1, _temp, FColor::White, FString::SanitizeFloat(objectYaw) + ", "+ FString::FromInt(FMath::TruncToInt(objectYaw)) +", " + FString::FromInt(objectFlipBook->GetNumFrames()));
		objectSprite->SetSprite(objectFlipBook->GetSpriteAtFrame(FMath::TruncToInt(objectYaw)));
	}
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
	if (IsValid(_spaceState) && _dealingFaction == Faction::Player)
		_spaceState->ApplyRelation(_dealingFaction, _remainDamage);

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

		if (IsValid(_userState) && IsValid(_spaceState) && _dealingFaction == Faction::Player) {
			_peerResult = _spaceState->PeerIdentify(faction, _dealingFaction, true);
			//전략 포인트의 일부 가중치를 팩션 관계도에 반영
			_spaceState->ApplyRelation(_dealingFaction, strategicPoint, true);
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

	if (GetWorld()) {
		if(this->GetObjectType() == ObjectType::Ship && IsValid(UGameplayStatics::GetGameState(GetWorld())) && UGameplayStatics::GetGameState(GetWorld())->IsA(ASpaceState::StaticClass()))
			Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->AccumulateToShipCapacity(true);
		if(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()) && UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()->IsA(ASpaceHUDBase::StaticClass()))
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