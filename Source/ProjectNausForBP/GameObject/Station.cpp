// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Station.h"

AStation::AStation() {
	//objectCollision->Mobility = EComponentMobility::Static;
	DockingSlot = TArray<FDockSlot>();
	DockingClassMapper = TMap<ShipClass, TArray<int>>();
	objectSprite->SetWorldScale3D(FVector(_define_StructureScale, _define_StructureScale, _define_StructureScale));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = _define_StructureTick;
}

#pragma region Event Call
void AStation::BeginPlay() {
	Super::BeginPlay();

	UE_LOG(LogClass, Log, TEXT("[Info][Station][Spawn] Spawn Finish!"));
}

void AStation::Tick(float DeltaSeconds) {

	currentShield = FMath::Clamp(currentShield + rechargeShield * DeltaSeconds, 0.0f, maxShield);
	currentArmor = FMath::Clamp(currentArmor + repairArmor * DeltaSeconds, 0.0f, maxArmor);
	currentHull = FMath::Clamp(currentHull + repairHull * DeltaSeconds, 0.0f, maxHull);

	if (structureInfo != nullptr) {
		structureInfo->structureShieldRate = FMath::Clamp(currentShield / maxShield, 0.0f, 1.0f);
		structureInfo->structureArmorRate = FMath::Clamp(currentArmor / maxArmor, 0.0f, 1.0f);
		structureInfo->structureHullRate = FMath::Clamp(currentHull / currentHull, 0.0f, 1.0f);
	}
}

float AStation::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
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
			for (auto& item : structureInfo->itemSlot) {
				//모든 카고를 절반의 확률로 드랍
				if (FMath::FRandRange(_define_ChanceRandomMIN, _define_ChanceRandomMAX) > 0.5f)
					continue;

				_cargoContainer = Cast<ACargoContainer>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ACargoContainer::StaticClass(),
					FTransform(this->GetActorRotation(), this->GetActorLocation()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
				if (_cargoContainer) {
					_cargoContainer->InitObject(FMath::RandRange(0, 13));
					_cargoContainer->SetCargo(FItem(item.Key, item.Value));
					UGameplayStatics::FinishSpawningActor(_cargoContainer, _cargoContainer->GetTransform());
				}
			}
			structureInfo->itemSlot.Empty();
		}
		Destroy();
	}
	UE_LOG(LogClass, Log, TEXT("[Info][Station][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"),
		*this->GetName(), *DamageEvent.DamageTypeClass->GetName(), _remainDamage, *DamageCauser->GetName(), _effectShieldDamage, _effectArmorDamage, _effectHullDamage,
		_isCritical ? TEXT("Critical") : TEXT("Non Critical"));
	return _effectShieldDamage + _effectArmorDamage + _effectHullDamage;
}

void AStation::BeginDestroy() {
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int AStation::GetObjectID() const {
	if(structureInfo != nullptr)
		return structureInfo->structureID; 
	else return -1;
}

ObjectType AStation::GetObjectType() const {
	return ObjectType::Station;
}

Faction AStation::GetFaction() const {
	if (structureInfo != nullptr)
		return structureInfo->structureFaction; 
	else return Faction::Neutral;
}

void AStation::SetFaction(const Faction setFaction) {
	return;
}

BehaviorState AStation::GetBehaviorState() const {
	return BehaviorState::Idle;
}

bool AStation::InitObject(const int objectId) {
	return false;
}

bool AStation::LoadBaseObject(const float shield, const float armor, const float hull, const float power) {
	return false;
}

float AStation::GetValue(const GetStatType statType) const {
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

void AStation::GetRepaired(const GetStatType statType, float repairValue) {

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
const FString AStation::GetDestinationName() const {
	return "";
}

StructureType AStation::GetStationType() const {
	if(structureInfo != nullptr)
		return structureInfo->structureType;
	else return StructureType::TradingCenter;
}

bool AStation::RequestedDock(const Faction requestFaction, const ShipClass requestClass, FVector& slotLocation, FRotator& slotRotation) {
	if (!structureInfo || DockingSlot.Num() < 1 || !DockingClassMapper.Contains(requestClass))
		return false;
	if (Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()))->PeerIdentify(faction, requestFaction, false) < Peer::Boundary)
		return false;

	//도킹 가능한 슬롯에서 랜덤으로 얻어낸 슬롯 인덱스
	int _slotIndex = DockingClassMapper[requestClass][FMath::RandRange(0, DockingClassMapper[requestClass].Num() - 1)];

	for (ShipClass& availableClass : DockingSlot[FMath::Clamp(_slotIndex, 0, DockingSlot.Num() - 1)].dockAvailableClass) {
		if (availableClass == requestClass) {
			slotLocation = DockingSlot[FMath::Clamp(_slotIndex, 0, DockingSlot.Num() - 1)].dockPosition * _define_StructureScale;
			slotRotation = DockingSlot[FMath::Clamp(_slotIndex, 0, DockingSlot.Num() - 1)].dockDirection;
			return true;
		}
	}
	return false;
}

bool AStation::RequestedJump(const Faction requestFaction) const {
	return false;
}

bool AStation::SetStructureData(UPARAM(ref) FStructureInfo& structureData) {
	if (isInited)
		return false;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FStationData _tempStationData;

	structureInfo = &structureData;
	_tempStationData = _tempInstance->GetStationData((structureInfo->structureID));
	objectName = _tempStationData.StationName;

	if (_tempStationData.FlipSprite) 
		objectSprite->SetSprite(_tempStationData.FlipSprite);
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

	DockingSlot = _tempStationData.DockingSlot;
	for (int index = 0; index < DockingSlot.Num(); index++) {
		for (ShipClass& shipClass : DockingSlot[index].dockAvailableClass) {
			if (!DockingClassMapper.Contains(shipClass))
				DockingClassMapper.Emplace(shipClass, TArray<int>());
			DockingClassMapper[shipClass].Emplace(index);
		}
	}
	for (FDockSlot& slot : DockingSlot)
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation() + GetActorRotation().RotateVector(slot.dockPosition), GetActorLocation() + (slot.dockDirection).RotateVector(FVector::ForwardVector) * 100.0f, 20.0f, FColor::White, false, 3600.0f, 0, 3.0f);
	isInited = true;
	return true;
}

void AStation::GetStructureData(FStructureInfo& structureData) const {
	structureData = *structureInfo;
	return;
}

void AStation::GetStructureCargo(TArray<FItem>& setArray, bool isPlayerCargo, CargoSortType sortType) const {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempItemData;

	if (structureInfo == nullptr || !IsValid(_tempInstance))
		return;
	TMap<int, int>* _tempItemMap;
	if (isPlayerCargo)
		_tempItemMap = &structureInfo->playerItemSlot;
	else
		_tempItemMap = &structureInfo->itemSlot;
	if (_tempItemMap == nullptr)
		return;

	setArray.Empty();
	setArray.Reserve(_tempItemMap->Num());

	TMap<int, ItemType> _tempSortMap_ItemTypeMode;
	TMap<int, ModuleType> _tempSortMap_ModuleTypeMode;
	TMap<int, ModuleSize> _tempSortMap_ModuleSizeMode;
	TMap<int, float> _tempSortMap_CargoValueMode;

	switch (sortType) {
	case CargoSortType::ItemTypeASE:
	case CargoSortType::ItemTypeDESC:
		for (auto& item : *_tempItemMap) {
			_tempItemData = _tempInstance->GetItemData(item.Key);
			_tempSortMap_ItemTypeMode.Emplace(item.Key, _tempItemData.Type);
		}
		if (sortType == CargoSortType::ItemTypeASE)
			_tempSortMap_ItemTypeMode.ValueSort([](const ItemType& ElementA, const ItemType& ElementB) { return ElementA > ElementB; });
		else
			_tempSortMap_ItemTypeMode.ValueSort([](const ItemType& ElementA, const ItemType& ElementB) { return ElementA < ElementB; });
		for (auto& item : _tempSortMap_ItemTypeMode)
			setArray.Emplace(FItem(item.Key, (*_tempItemMap)[item.Key]));
		break;
	case CargoSortType::ModuleTypeASE:
	case CargoSortType::ModuleTypeDESC:
		for (auto& item : *_tempItemMap) {
			_tempItemData = _tempInstance->GetItemData(item.Key);
			_tempSortMap_ModuleTypeMode.Emplace(item.Key, _tempItemData.ModuleType);
		}
		if (sortType == CargoSortType::ItemTypeASE)
			_tempSortMap_ModuleTypeMode.ValueSort([](const ModuleType& ElementA, const ModuleType& ElementB) { return ElementA > ElementB; });
		else
			_tempSortMap_ModuleTypeMode.ValueSort([](const ModuleType& ElementA, const ModuleType& ElementB) { return ElementA < ElementB; });
		for (auto& item : _tempSortMap_ModuleTypeMode)
			setArray.Emplace(FItem(item.Key, (*_tempItemMap)[item.Key]));
		break;
	case CargoSortType::ModuleSizeASE:
	case CargoSortType::ModuleSizeDESC:
		for (auto& item : *_tempItemMap) {
			_tempItemData = _tempInstance->GetItemData(item.Key);
			_tempSortMap_ModuleSizeMode.Emplace(item.Key, _tempItemData.ModuleSize);
		}
		if (sortType == CargoSortType::ItemTypeASE)
			_tempSortMap_ModuleSizeMode.ValueSort([](const ModuleSize& ElementA, const ModuleSize& ElementB) { return ElementA > ElementB; });
		else
			_tempSortMap_ModuleSizeMode.ValueSort([](const ModuleSize& ElementA, const ModuleSize& ElementB) { return ElementA < ElementB; });
		for (auto& item : _tempSortMap_ModuleSizeMode)
			setArray.Emplace(FItem(item.Key, (*_tempItemMap)[item.Key]));
		break;
	case CargoSortType::CargoValueSizeASE:
	case CargoSortType::CargoValueSizeDESC:
		for (auto& item : *_tempItemMap) {
			_tempItemData = _tempInstance->GetItemData(item.Key);
			_tempSortMap_CargoValueMode.Emplace(item.Key, _tempItemData.CargoVolume);
		}
		if (sortType == CargoSortType::ItemTypeASE)
			_tempSortMap_CargoValueMode.ValueSort([](const float& ElementA, const float& ElementB) { return ElementA > ElementB; });
		else
			_tempSortMap_CargoValueMode.ValueSort([](const float& ElementA, const float& ElementB) { return ElementA < ElementB; });
		for (auto& item : _tempSortMap_CargoValueMode)
			setArray.Emplace(FItem(item.Key, (*_tempItemMap)[item.Key]));
		break;
	default:
		for (auto& item : *_tempItemMap)
			setArray.Emplace(FItem(item.Key, item.Value));
		return;
		break;
	}
}
int AStation::FindStructureCargoAmount(const int findItemID, bool isPlayerCargo) const {
	if (structureInfo == nullptr)
		return -1;
	if (isPlayerCargo && structureInfo->playerItemSlot.Contains(findItemID))
		return structureInfo->playerItemSlot[findItemID];
	if (!isPlayerCargo && structureInfo->itemSlot.Contains(findItemID)) 
		return structureInfo->itemSlot[findItemID];
	return -1;
}
#pragma endregion

#pragma region Functions
FStructureInfo* AStation::GetStructureDataPointer() const {
	return structureInfo;
}

#pragma endregion