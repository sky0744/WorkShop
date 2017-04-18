// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "ProjectNausForBP.h"
#include "SafeENGINE.h"

#define limitHalfSizeSector 99000.0f

USafeENGINE::USafeENGINE() {
	static ConstructorHelpers::FObjectFinder<UDataTable> _ShipDataTable(TEXT("DataTable'/Game/DataTable/ShipData.ShipData'"));
	ShipData = _ShipDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _NPCDataTable(TEXT("DataTable'/Game/DataTable/NPCData.NPCData'"));
	NPCData = _NPCDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _StationDataTable(TEXT("DataTable'/Game/DataTable/StationData.StationData'"));
	StationData = _StationDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _ResourceDataTable(TEXT("DataTable'/Game/DataTable/ResourceData.ResourceData'"));
	ResourceData = _ResourceDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> _SkillDataTable(TEXT("DataTable'/Game/DataTable/SkillData.SkillData'"));
	SkillData = _SkillDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _ItemDataTable(TEXT("DataTable'/Game/DataTable/ItemData.ItemData'"));
	ItemData = _ItemDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _ProjectileDataTable(TEXT("DataTable'/Game/DataTable/ProjectileData.ProjectileData'"));
	ProjectileData = _ProjectileDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _SectorDataTable(TEXT("DataTable'/Game/DataTable/SectorData.SectorData'"));
	SectorData = _SectorDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _StartPlayerTable(TEXT("DataTable'/Game/DataTable/StartProfile.StartProfile'"));
	StartPlayerData = _StartPlayerTable.Object;
}

#pragma region Get DataTable
FShipData USafeENGINE::GetShipData(int id) {
	id = FMath::Max(id, 0);
	FShipData* _data = ShipData->FindRow<FShipData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ShipData->FindRow<FShipData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

FNPCData USafeENGINE::GetNPCData(int id) {
	id = FMath::Max(id, 0);
	FNPCData* _data = NPCData->FindRow<FNPCData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *NPCData->FindRow<FNPCData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

FStationData USafeENGINE::GetStationData(int id) {
	id = FMath::Max(id, 0);
	FStationData* _data = StationData->FindRow<FStationData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *StationData->FindRow<FStationData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

FResourceData USafeENGINE::GetResourceData(int id) {
	id = FMath::Max(id, 0);
	FResourceData* _data = ResourceData->FindRow<FResourceData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ResourceData->FindRow<FResourceData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}


FSkillData USafeENGINE::GetSkillData(int id) {
	id = FMath::Max(id, 0);
	FSkillData* _data = SkillData->FindRow<FSkillData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *SkillData->FindRow<FSkillData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

FItemData USafeENGINE::GetItemData(int id) {
	id = FMath::Max(id, 0);
	FItemData* _data = ItemData->FindRow<FItemData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ItemData->FindRow<FItemData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

FProjectileData USafeENGINE::GetProjectileData(int id) {
	id = FMath::Max(id, 0);
	FProjectileData* _data = ProjectileData->FindRow<FProjectileData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ProjectileData->FindRow<FProjectileData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

FSectorData USafeENGINE::GetSectorData(FString SectorName) {

	TArray<FString> _sectorNames;
	int _selectedIndex = -1;

	GetAllSectorNameData(_sectorNames);

	for (int index = 0; index < _sectorNames.Num(); index++) {
		if (SectorName.Compare(_sectorNames[index], ESearchCase::IgnoreCase) == 0) {
			_selectedIndex = index;
			break;
		}
	}

	if(_selectedIndex > -1)
		return *SectorData->FindRow<FSectorData>(*_sectorNames[_selectedIndex], TEXT("DataTableLoadError"));
	else 
		return *SectorData->FindRow<FSectorData>("Cansoe", TEXT("DataTableLoadError"));
}

void USafeENGINE::GetAllSectorNameData(TArray<FString>& sectorNameArray) {
	sectorNameArray.Empty();
	TArray<FName> _sectorNames = SectorData->GetRowNames();

	sectorNameArray.Reserve(_sectorNames.Num());
	for (int index = 0; index < _sectorNames.Num(); index++)
		sectorNameArray.Emplace(_sectorNames[index].ToString());
}

FNewStartPlayerData USafeENGINE::GetStartProfileData(int id) {
	FNewStartPlayerData* _data = StartPlayerData->FindRow<FNewStartPlayerData>(*FString::Printf(TEXT("%d"), id), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *StartPlayerData->FindRow<FNewStartPlayerData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}
#pragma endregion

#pragma region GamePlay
int USafeENGINE::FindItemSlot(UPARAM(ref) TArray<FItem>& itemList, FItem items) {
	for (int index = 0; index < itemList.Num(); index++) {
		if (itemList[index].itemID == items.itemID) {
			return index;
		}
	}
	return -1;
}

bool USafeENGINE::AddCargo(UPARAM(ref) TArray<FItem>& itemList, FItem addItem) {
	int _findItemSlot = -1;

	UE_LOG(LogClass, Log, TEXT("[AddCargo][ref Check] (&)%p"), &itemList);
	if (addItem.itemAmount < 1)
		return false;

	_findItemSlot = FindItemSlot(itemList, addItem);
	if (_findItemSlot > -1) {
		itemList[_findItemSlot].itemAmount += addItem.itemAmount;
		UE_LOG(LogClass, Log, TEXT("[AddCargo][Find] %d"), itemList.Num());
	}
	else {
		itemList.Emplace(FItem(addItem.itemID, addItem.itemAmount));
		UE_LOG(LogClass, Log, TEXT("[AddCargo][Generate] %d"), itemList.Num());
	}

	return true;
}

bool USafeENGINE::DropCargo(UPARAM(ref) TArray<FItem>& itemList, FItem dropItem) {
	int _slotIndex = -1;

	if (dropItem.itemAmount < 1)
		return false;

	UE_LOG(LogClass, Log, TEXT("[DropCargo][ref Check] %p"), &itemList);
	_slotIndex = FindItemSlot(itemList, dropItem);
	if (_slotIndex > -1) {
		if (itemList[_slotIndex].itemAmount > dropItem.itemAmount) {
			itemList[_slotIndex].itemAmount -= dropItem.itemAmount;
			UE_LOG(LogClass, Log, TEXT("[DropCargo][Find] %d"), itemList.Num());
		}
		else if (itemList[_slotIndex].itemAmount == dropItem.itemAmount) {
			itemList.RemoveAt(_slotIndex);
			UE_LOG(LogClass, Log, TEXT("[DropCargo][Delete] %d"), itemList.Num());
		}
		else {
			UE_LOG(LogClass, Log, TEXT("[DropCargo][Can't] %d"), itemList.Num());
			return false;
		}
	}
	else return false;

	return true;
}

float USafeENGINE::CheckDistacneConsiderSize(ASpaceObject* actor1, ASpaceObject* actor2) {
	if (actor1 == nullptr || actor2 == nullptr)
		return 0.0f;

	float _realDistance = FVector::Dist(actor1->GetActorLocation(), actor2->GetActorLocation());
	_realDistance -= actor1->GetValue(GetStatType::halfLength);
	_realDistance -= actor2->GetValue(GetStatType::halfLength);

	return FMath::Max(0.0f, _realDistance);
}

FVector USafeENGINE::CheckLocationMovetoTarget(ASpaceObject* requestor, ASpaceObject* target, float distance) {
	if (requestor == nullptr || target == nullptr)
		return FVector::ZeroVector;

	FVector _location = target->GetActorLocation();
	FVector _directiveVector = _location - requestor->GetActorLocation();
	_directiveVector.Normalize();

	distance += requestor->GetValue(GetStatType::halfLength);
	distance += target->GetValue(GetStatType::halfLength);
	distance -= FMath::FRandRange(50.0f, 100.0f);

	_location -= _directiveVector * distance;
	_location.X = FMath::Clamp(_location.X, -limitHalfSizeSector, limitHalfSizeSector);
	_location.Y = FMath::Clamp(_location.Y, -limitHalfSizeSector, limitHalfSizeSector);
	_location.Z = 0.0f;
	
	return _location;
}

FVector USafeENGINE::GetRandomLocationToTarget(ASpaceObject* requestor, ASpaceObject* target, float distance) {
	if (requestor == nullptr || target == nullptr)
		return FVector::ZeroVector;

	FVector _location = target->GetActorLocation();
	FVector _directiveVector = FMath::VRand().GetSafeNormal();

	distance += requestor->GetValue(GetStatType::halfLength);
	distance += target->GetValue(GetStatType::halfLength);
	distance -= FMath::FRandRange(50.0f, 100.0f);

	_location -= _directiveVector * distance;
	_location.X = FMath::Clamp(_location.X, -limitHalfSizeSector, limitHalfSizeSector);
	_location.Y = FMath::Clamp(_location.Y, -limitHalfSizeSector, limitHalfSizeSector);
	_location.Z = 0.0f;

	return _location;
}

FVector USafeENGINE::GetRandomLocationToLocation(FVector location, float distance) {

	FVector _directiveVector;
	_directiveVector.X = FMath::FRandRange(-limitHalfSizeSector, limitHalfSizeSector);
	_directiveVector.Y = FMath::FRandRange(-limitHalfSizeSector, limitHalfSizeSector);
	_directiveVector.Z = 0.0f;
	_directiveVector.Normalize();

	return location + _directiveVector * distance;
}

FVector USafeENGINE::GetRandomLocation(bool requestNormalizedVector) {

	FVector _location;

	_location.X = FMath::FRandRange(-limitHalfSizeSector, limitHalfSizeSector);
	_location.Y = FMath::FRandRange(-limitHalfSizeSector, limitHalfSizeSector);
	_location.Z = 0.0f;

	if (requestNormalizedVector)
		_location.Normalize();

	return _location;
}

FVector USafeENGINE::GetLocationLimitedSector(FVector location) {
	
	location.X = FMath::Clamp(location.X, -limitHalfSizeSector, limitHalfSizeSector);
	location.Y = FMath::Clamp(location.Y, -limitHalfSizeSector, limitHalfSizeSector);
	location.Z = 0.0f;

	return location;
}

float USafeENGINE::CalculateCreditForTrade(int itemID, int lowerAmount, int upperAmount, bool isBuy) {
	float _totalValue;
	int _tempValue;
	int _partMaxValueAmount;		int _partMinValueAmount;		int _partDynamicAmount;
	float _valueInLowerAmount;	float _valueInUpperAmount;
	int _SellValueMaxAmount;		int _SellValueMinAmount;
	float _SellValueMax;			float _SellValueMin;
	FItemData _itemData = this->GetItemData(itemID);

	_SellValueMaxAmount = FMath::Max(_itemData.PointAmount1, _itemData.PointAmount2);
	_SellValueMinAmount = FMath::Min(_itemData.PointAmount1, _itemData.PointAmount2);
	_SellValueMax = FMath::Max(_itemData.PointCredit1, _itemData.PointCredit2);
	_SellValueMin = FMath::Min(_itemData.PointCredit1, _itemData.PointCredit2);

	_tempValue = FMath::Min(lowerAmount, upperAmount);
	upperAmount = FMath::Max(lowerAmount, upperAmount);
	lowerAmount = _tempValue;

	_partMaxValueAmount = FMath::Max(0, FMath::Min(upperAmount, _SellValueMinAmount) - FMath::Min(lowerAmount, _SellValueMinAmount));
	_partMinValueAmount = FMath::Max(0, FMath::Max(upperAmount, _SellValueMaxAmount) - FMath::Max(lowerAmount, _SellValueMaxAmount));
	_partDynamicAmount = FMath::Max(0, upperAmount - (lowerAmount + _partMaxValueAmount + _partMinValueAmount));

	_totalValue = FMath::Max(0.0f, _SellValueMax * _partMaxValueAmount);
	_totalValue += FMath::Max(0.0f, _SellValueMin * _partMinValueAmount);
	_tempValue = _SellValueMaxAmount - _SellValueMinAmount;

	_valueInLowerAmount = _SellValueMin + (_SellValueMax - _SellValueMin) * FMath::Clamp((_SellValueMaxAmount - FMath::Min(upperAmount, _SellValueMaxAmount)) / (float)_tempValue, .0f, 1.0f);
	_valueInUpperAmount = _SellValueMin + (_SellValueMax - _SellValueMin) * FMath::Clamp((_SellValueMaxAmount - FMath::Max(lowerAmount, _SellValueMinAmount)) / (float)_tempValue, .0f, 1.0f);
	_totalValue += (_valueInLowerAmount + _valueInUpperAmount) * 0.5f * _partDynamicAmount;

	if (isBuy)
		_totalValue *= FMath::Clamp(_itemData.BuyValueMultiple, 1.1f, 2.0f);
	return _totalValue;
}

float USafeENGINE::CalculateCredit(int itemID, int Amount, bool isBuy) {
	FItemData _itemData = this->GetItemData(itemID);
	float _result;
	int _SellValueMaxAmount;		int _SellValueMinAmount;
	float _SellValueMax;			float _SellValueMin;

	_SellValueMaxAmount = FMath::Max(_itemData.PointAmount1, _itemData.PointAmount2);
	_SellValueMinAmount = FMath::Min(_itemData.PointAmount1, _itemData.PointAmount2);
	_SellValueMax = FMath::Max(_itemData.PointCredit1, _itemData.PointCredit2);
	_SellValueMin = FMath::Min(_itemData.PointCredit1, _itemData.PointCredit2);

	_result = _SellValueMin + (_SellValueMax - _SellValueMin) * FMath::Clamp((_SellValueMaxAmount - Amount) / (float)(_SellValueMaxAmount - _SellValueMinAmount), .0f, 1.0f);
	
	if (isBuy)
		_result *= FMath::Clamp(_itemData.BuyValueMultiple, 1.1f, 2.0f);
	return _result;
}

bool USafeENGINE::CheckSkill(UPARAM(ref) TArray<FSkill>& skillList, UPARAM(ref) TArray<FSkill>& requsetCheckSkillList) {
	/*bool tempCheckSu
	for (int index = 0; index < skillList.Num(); index++) {
		for (int index1 = 0; index1 < requsetCheckSkillList.Num(); index1++) {
			if (skillList[index].skillID == requsetCheckSkillList[index].skillID) {
				if (skillList[index].skillLevel >= requsetCheckSkillList[index].skillLevel)
					return true;
				else return false;
			}
		}
	}*/
	return false;
}
#pragma endregion

#pragma region GamePlay - Non-Static

#pragma endregion