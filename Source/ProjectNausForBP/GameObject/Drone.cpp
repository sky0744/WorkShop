// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Drone.h"

ADrone::ADrone() {
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
			if (_dealingFaction == Faction::Player) {
				_userState->ChangeRenown(_peerResult, strategicPoint);
			}
		}
		/*
		//카고 드랍
		ACargoContainer* _cargoContanier;
		USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
		FNPCData _tempNPCData = _tempInstance->GetNPCData(npcShipID);

		for (FNPCDropData& dropData : _tempNPCData.DropItems) {
			if (FMath::Clamp(dropData.dropChance, _define_DropChance_MIN, _define_DropChance_MAX) < FMath::FRandRange(_define_DropChance_MIN, _define_DropChance_MAX))
				continue;

			_cargoContanier = Cast<ACargoContainer>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ACargoContainer::StaticClass(),
				FTransform(this->GetActorRotation(), this->GetActorLocation()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

			if (_cargoContanier) {
			_cargoContainer->InitObject(FMath::RandRange(0, 13));
				_cargoContanier->SetCargo(FItem(dropData.dropItemID, FMath::RandRange(dropData.dropAmountMin, dropData.dropAmountMax)));
				UGameplayStatics::FinishSpawningActor(_cargoContanier, _cargoContanier->GetTransform());
			}
		}*/
		Destroy();
	}

	UE_LOG(LogClass, Log, TEXT("[Info][Drone][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage,
		_isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void ADrone::BeginDestroy() {
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

	FShipData tempData = tmpInstance->GetShipData(objectId);
	if (&tempData == nullptr)
	return false;

	if (sShipID.GetValue() != objectId) {
	sShipID.SetValue(objectId);
	objectName = ...?
	if (_tempDroneData.FlipSprite){
	objectFlipBook = _tempShipData.FlipSprite;
	objectSprite->SetSprite(objectFlipBook->GetSpriteAtFrame(0));
	}

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
