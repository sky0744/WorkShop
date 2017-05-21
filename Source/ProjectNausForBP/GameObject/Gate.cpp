// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Gate.h"

AGate::AGate()
{
	objectCollision->Mobility = EComponentMobility::Static;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = _define_StructureTick;
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

	if (structureInfo != nullptr) {
		structureInfo->structureShieldRate = FMath::Clamp(currentShield / maxShield, 0.0f, 1.0f);
		structureInfo->structureArmorRate = FMath::Clamp(currentArmor / maxArmor, 0.0f, 1.0f);
		structureInfo->structureHullRate = FMath::Clamp(currentHull / currentHull, 0.0f, 1.0f);
	}
}

float AGate::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	Faction _dealingFaction;

	if (DamageCauser->IsA(ABeam::StaticClass()))
		_dealingFaction = Cast<ABeam>(DamageCauser)->GetLaunchingFaction();
	else if (DamageCauser->IsA(AProjectiles::StaticClass()))
		_dealingFaction = Cast<AProjectiles>(DamageCauser)->GetLaunchingFaction();
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
	float _effectReduceDamage = 0.0f;
	bool _isCritical = false;

	if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) > 0.95f) {
		_remainDamage *= 2.0f;
		_isCritical = true;
	} else if (FVector::DotProduct(GetActorForwardVector(), _hitDirect) < -0.95f) {
		_remainDamage *= 3.0f;
		_isCritical = true;
	}

	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	if (IsValid(_spaceState) && _dealingFaction == Faction::Player)
		_spaceState->ApplyRelation(_dealingFaction, _remainDamage);

	/*
	*	데미지 경감 공식 : 경감률 = (def -1)^2 + 0.15,
	*	Def 범위 : -1000.0f ~ 1000.0f -> 최대 경감률 : -4.15f(역경감) ~ 0.15f
	*/
	if (currentShield > _remainDamage) {
		_effectReduceDamage = FMath::Pow((FMath::Clamp(defShield, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
		_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
		_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
		_effectShieldDamage = _remainDamage;
		currentShield -= _remainDamage;
		_remainDamage = 0.0f;
	} else {
		_effectShieldDamage = currentShield;
		_remainDamage -= _effectShieldDamage;
		currentShield = 0.0f;

		if (currentArmor > _remainDamage) {
			_effectReduceDamage = FMath::Pow((FMath::Clamp(defArmor, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
			_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
			_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
			_effectArmorDamage = _remainDamage;
			currentArmor -= _remainDamage;
			_remainDamage = 0.0f;
		} else {
			_effectArmorDamage = currentArmor;
			_remainDamage -= _effectArmorDamage;
			currentArmor = 0.0f;

			_effectReduceDamage = FMath::Pow((FMath::Clamp(defHull, _define_StatDefMIN, _define_StatDefMAX) / _define_StatDefMAX - 1.0f), 2.0f);
			_effectReduceDamage = FMath::Clamp(_effectReduceDamage + 0.15f, _define_DamagePercentageMIN, _define_DamagePercentageMAX);
			_remainDamage = FMath::Clamp(_remainDamage * _effectReduceDamage, _define_DamagedMIN, _define_DamagedMAX);
			_effectHullDamage = _remainDamage;
			currentHull -= _remainDamage;
			_remainDamage = 0.0f;
		}
	}

	if (currentHull <= 0.0f) {
		_effectHullDamage = currentHull;
		currentHull = 0.0f;

		AUserState* _userState = Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState);
		Peer _peerResult = Peer::Neutral;

		if (IsValid(_userState) && IsValid(_spaceState) && _dealingFaction == Faction::Player) {
			_peerResult = _spaceState->PeerIdentify(faction, _dealingFaction, true);
			//전략 포인트의 일부 가중치를 팩션 관계도에 반영
			_spaceState->ApplyRelation(_dealingFaction, strategicPoint, true);
			if (_dealingFaction == Faction::Player)
				_userState->ChangeRenown(_peerResult, strategicPoint);
		}
		//카고 드랍
		ACargoContainer* _cargoContainer;
		if (structureInfo != nullptr) {
			for (FItem& cargo : structureInfo->itemList) {
				//모든 카고를 절반의 확률로 드랍
				if (FMath::FRandRange(_define_DropChanceMIN, _define_DropChanceMAX) > 0.5f)
					continue;

				_cargoContainer = Cast<ACargoContainer>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ACargoContainer::StaticClass(),
					FTransform(this->GetActorRotation(), this->GetActorLocation()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
				if (_cargoContainer) {
					_cargoContainer->InitObject(FMath::RandRange(0, 13));
					_cargoContainer->SetCargo(cargo);
					UGameplayStatics::FinishSpawningActor(_cargoContainer, _cargoContainer->GetTransform());
				}
			}
			structureInfo->itemList.Empty();
		}
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Gate][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage,
		_isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void AGate::BeginDestroy() {
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int AGate::GetObjectID() const {
	if (structureInfo != nullptr)
		return structureInfo->structureID;
	else return -1;
}

ObjectType AGate::GetObjectType() const {
	return ObjectType::Gate;
}

Faction AGate::GetFaction() const {
	if (structureInfo != nullptr)
		return structureInfo->structureFaction;
	else return Faction::Neutral;
}

void AGate::SetFaction(const Faction setFaction) {

	return;
}

BehaviorState AGate::GetBehaviorState() const {
	return BehaviorState::Idle;
}

bool AGate::InitObject(const int objectId) {
	return false;
}

bool AGate::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {
	return false;
}

float AGate::GetValue(const GetStatType statType) const {
	float _value;

	switch (statType) {
	case GetStatType::halfLength:
		_value = objectCollision->GetScaledSphereRadius() * 0.5f;
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
	case GetStatType::repaireArmor:
		_value = repairArmor;
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
		_value = repairHull;
		break;
	case GetStatType::currentHull:
		_value = currentHull;
		break;
	case GetStatType::defHull:
		_value = defHull;
		break;
	default:
		_value = 0.0f;
		break;
	}

	return _value;
}

void AGate::GetRepaired(const GetStatType statType, float repairValue) {

	repairValue = FMath::Clamp(repairValue, _define_StatRestoreMIN, _define_StatRestoreMAX);
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
const FString AGate::GetDestinationName() const {
	if (structureInfo != nullptr)
		return structureInfo->LinkedSector;
	else return "";
}

StructureType AGate::GetStationType() const {
	return StructureType::Gate;
}

bool AGate::RequestedDock(const Faction requestFaction) const {
	if (structureInfo != nullptr)
		return true;
	else return false;
}
bool AGate::RequestedJump(const Faction requestFaction) const {
	if(structureInfo != nullptr)
		return true;
	else return false;
}

bool AGate::SetStructureData(UPARAM(ref) FStructureInfo& structureData) {
	if (isInited)
		return false;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FStationData _tempStationData;
	
	structureInfo = &structureData;
	_tempStationData = _tempInstance->GetStationData(structureInfo->structureID);
	objectName = _tempStationData.Name;

	UPaperFlipbook* _newFlipBook = Cast<UPaperFlipbook>(StaticLoadObject(UPaperFlipbook::StaticClass(), NULL, *_tempStationData.SpritePath.ToString()));
	if (_newFlipBook)
		objectFlipBook->SetFlipbook(_newFlipBook);
	strategicPoint = FMath::Clamp(_tempStationData.StrategicPoint, _define_StatStrategicPointMIN, _define_StatStrategicPointMAX);

	maxShield = _tempStationData.Shield;
	currentShield = structureInfo->structureShieldRate * maxShield;
	rechargeShield = _tempStationData.RechargeShield;
	defShield = _tempStationData.DefShield;

	maxArmor = _tempStationData.Armor;
	currentArmor = structureInfo->structureArmorRate * maxArmor;
	repairArmor = _tempStationData.RepairArmor;
	defArmor = _tempStationData.DefArmor;

	maxHull = _tempStationData.Hull;
	currentHull = structureInfo->structureHullRate * maxHull;
	repairHull = _tempStationData.RepairHull;
	defHull = _tempStationData.DefHull;

	isInited = true;
	return true;
}

void AGate::GetStructureData(FStructureInfo& structureData) const {
	structureData = *structureInfo;
	return;
}
#pragma endregion

#pragma region Functions
FStructureInfo* AGate::GetStructureDataPointer() const{
	return structureInfo;
}

#pragma endregion
