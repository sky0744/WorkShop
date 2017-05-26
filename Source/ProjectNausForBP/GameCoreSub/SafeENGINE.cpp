// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SafeENGINE.h"

USafeENGINE::USafeENGINE() {
	static ConstructorHelpers::FObjectFinder<UDataTable> _ShipDataTable(TEXT("DataTable'/Game/DataTable/ShipData.ShipData'"));
	ShipData = _ShipDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _NPCDataTable(TEXT("DataTable'/Game/DataTable/NPCData.NPCData'"));
	NPCData = _NPCDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _StationDataTable(TEXT("DataTable'/Game/DataTable/StationData.StationData'"));
	StationData = _StationDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _ResourceDataTable(TEXT("DataTable'/Game/DataTable/ResourceData.ResourceData'"));
	ResourceData = _ResourceDataTable.Object;
	ConstructorHelpers::FObjectFinder<UDataTable> _ContainerDataTable(TEXT("DataTable'/Game/DataTable/CargoContainerData.CargoContainerData'"));
	ContainerData = _ContainerDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _ObjectDataTable(TEXT("DataTable'/Game/DataTable/ObjectData.ObjectData'"));
	ObjectData = _ObjectDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> _SkillDataTable(TEXT("DataTable'/Game/DataTable/SkillData.SkillData'"));
	SkillData = _SkillDataTable.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> _QuestDataTable(TEXT("DataTable'/Game/DataTable/QuestData.QuestData'"));
	QuestData = _QuestDataTable.Object;
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
const FShipData& USafeENGINE::GetShipData(const int& id) const {
	FShipData* _data = ShipData->FindRow<FShipData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ShipData->FindRow<FShipData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FNPCData& USafeENGINE::GetNPCData(const int& id) const {
	FNPCData* _data = NPCData->FindRow<FNPCData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *NPCData->FindRow<FNPCData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FStationData& USafeENGINE::GetStationData(const int& id) const {
	FStationData* _data = StationData->FindRow<FStationData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *StationData->FindRow<FStationData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FResourceData& USafeENGINE::GetResourceData(const int& id) const {
	FResourceData* _data = ResourceData->FindRow<FResourceData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ResourceData->FindRow<FResourceData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FObjectData& USafeENGINE::GetCargoContainerData(const int& id) const {

	FObjectData* _data = ContainerData->FindRow<FObjectData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ContainerData->FindRow<FObjectData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FObjectData& USafeENGINE::GetObjectData(const int& id) const {
	
	FObjectData* _data = ObjectData->FindRow<FObjectData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ObjectData->FindRow<FObjectData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FSkillData& USafeENGINE::GetSkillData(const int& id) const {
	FSkillData* _data = SkillData->FindRow<FSkillData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *SkillData->FindRow<FSkillData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FQuestData& USafeENGINE::GetQuestData(const int& id) const {
	FQuestData* _data = QuestData->FindRow<FQuestData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *QuestData->FindRow<FQuestData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FItemData& USafeENGINE::GetItemData(const int& id) const {
	FItemData* _data = ItemData->FindRow<FItemData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ItemData->FindRow<FItemData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FProjectileData& USafeENGINE::GetProjectileData(const int& id) const {
	FProjectileData* _data = ProjectileData->FindRow<FProjectileData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *ProjectileData->FindRow<FProjectileData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}

const FSectorData& USafeENGINE::GetSectorData(const FString& SectorName) const {

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

void USafeENGINE::GetAllSectorNameData(TArray<FString>& sectorNameArray) const {
	sectorNameArray.Empty();
	TArray<FName> _sectorNames = SectorData->GetRowNames();

	sectorNameArray.Reserve(_sectorNames.Num());
	for (int index = 0; index < _sectorNames.Num(); index++)
		sectorNameArray.Emplace(_sectorNames[index].ToString());
}

const FNewStartPlayerData& USafeENGINE::GetStartProfileData(const int& id) const {
	FNewStartPlayerData* _data = StartPlayerData->FindRow<FNewStartPlayerData>(*FString::Printf(TEXT("%d"), FMath::Max(id, 0)), TEXT("DataTableLoadError"));
	if (_data)
		return *_data;
	else return *StartPlayerData->FindRow<FNewStartPlayerData>(*FString::Printf(TEXT("%d"), 0), TEXT("DataTableLoadError"));
}
#pragma endregion

#pragma region GamePlay
FVector USafeENGINE::GetRandomLocationToLocation(FVector location, float distance) {

	FVector _directiveVector;
	_directiveVector.X = FMath::FRandRange(_define_LimitSectorSizeMIN, _define_LimitSectorSizeMAX);
	_directiveVector.Y = FMath::FRandRange(_define_LimitSectorSizeMIN, _define_LimitSectorSizeMAX);
	_directiveVector.Z = 0.0f;
	_directiveVector.Normalize();

	return location + _directiveVector * distance;
}

FVector USafeENGINE::GetRandomLocation(bool requestNormalizedVector) {

	FVector _location;

	_location.X = FMath::FRandRange(_define_LimitSectorSizeMIN, _define_LimitSectorSizeMAX);
	_location.Y = FMath::FRandRange(_define_LimitSectorSizeMIN, _define_LimitSectorSizeMAX);
	_location.Z = 0.0f;

	if (requestNormalizedVector)
		_location.Normalize();

	return _location;
}

FVector USafeENGINE::GetLocationLimitedSector(FVector location) {
	
	location.X = FMath::Clamp(location.X, _define_LimitSectorSizeMIN, _define_LimitSectorSizeMAX);
	location.Y = FMath::Clamp(location.Y, _define_LimitSectorSizeMIN, _define_LimitSectorSizeMAX);
	location.Z = 0.0f;

	return location;
}

#pragma endregion

#pragma region GamePlay - Non-Static
float USafeENGINE::CalculateCreditForTrade(int itemID, int lowerAmount, int upperAmount, bool isBuy) const {
	float _totalValue;
	int _tempValue;
	int _partMaxValueAmount;		int _partMinValueAmount;		int _partDynamicAmount;
	float _valueInLowerAmount;	float _valueInUpperAmount;
	int _SellValueMaxAmount;		int _SellValueMinAmount;
	float _SellValueMax;			float _SellValueMin;
	FItemData _itemData = this->GetItemData(itemID);

	_SellValueMaxAmount = FMath::Max(_itemData.AmountPointRange.X, _itemData.AmountPointRange.Y);
	_SellValueMinAmount = FMath::Min(_itemData.AmountPointRange.X, _itemData.AmountPointRange.Y);
	_SellValueMax = FMath::Max(_itemData.ValuePointRange.X, _itemData.ValuePointRange.Y);
	_SellValueMin = FMath::Min(_itemData.ValuePointRange.X, _itemData.ValuePointRange.Y);

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

float USafeENGINE::CalculateCredit(int itemID, int Amount, bool isBuy) const {
	float _result;
	int _SellValueMaxAmount;		int _SellValueMinAmount;
	float _SellValueMax;			float _SellValueMin;
	FItemData _itemData = this->GetItemData(itemID);

	_SellValueMaxAmount = FMath::Max(_itemData.AmountPointRange.X, _itemData.AmountPointRange.Y);
	_SellValueMinAmount = FMath::Min(_itemData.AmountPointRange.X, _itemData.AmountPointRange.Y);
	_SellValueMax = FMath::Max(_itemData.ValuePointRange.X, _itemData.ValuePointRange.Y);
	_SellValueMin = FMath::Min(_itemData.ValuePointRange.X, _itemData.ValuePointRange.Y);

	_result = _SellValueMin + (_SellValueMax - _SellValueMin) * FMath::Clamp((_SellValueMaxAmount - Amount) / (float)(_SellValueMaxAmount - _SellValueMinAmount), .0f, 1.0f);
	if (isBuy)
		_result *= FMath::Clamp(_itemData.BuyValueMultiple, 1.1f, 2.0f);
	return _result;
}
#pragma endregion