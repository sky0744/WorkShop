// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceState.h"


ASpaceState::ASpaceState() {

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 60.0f;

	sectorInfo = TArray<FSectorData>();
	tempFactionRelationship = TArray<float>();
	factionRelationship = TArray<float>();
}

void ASpaceState::BeginPlay() {
	Super::BeginPlay();

	factionEnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("Faction"), true);
	skipingFirstTick = true;
	currentShipCapacity = 0;
}

void ASpaceState::Tick(float DeltaSecondes) {
	Super::Tick(DeltaSecondes);

	if (currentSectorInfo == nullptr || skipingFirstTick) {
		skipingFirstTick = false;
		return;
	}

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FStationData _tempStationData;
	FNPCData _tempNPCData;
	FShipData _tempShipData;
	FItem _item = FItem();

	IStructureable* _regenStructure;
	TArray<AActor*> objectInSector;
	bool _canProduct;
	float _tempValue;
	int _findIndex;
	int _addAmount;
	int _totalFactorInTick = totalChanceFactor;

	for (FSectorData& sectorData : sectorInfo) {

		if (&sectorData == currentSectorInfo)
			continue;
		for (FStructureInfo& stationData : sectorData.StationList) {
			if (stationData.isDestroyed)
				stationData.remainRespawnTime = FMath::Clamp(stationData.remainRespawnTime - DeltaSecondes, 0.0f, stationData.maxRespawnTime);
			else {
				_tempValue = DeltaSecondes / stationData.maxRespawnTime;
				stationData.structureShieldRate = FMath::Clamp(stationData.structureShieldRate + _tempValue, 0.0f, 1.0f);
				stationData.structureArmorRate = FMath::Clamp(stationData.structureArmorRate + _tempValue, 0.0f, 1.0f);
				stationData.structureHullRate = FMath::Clamp(stationData.structureHullRate + _tempValue, 0.0f, 1.0f);

				stationData.remainItemListRefreshTime = FMath::Clamp(stationData.remainItemListRefreshTime - DeltaSecondes, 0.0f, stationData.maxItemListRefreshTime);
				stationData.remainProductTime = FMath::Clamp(stationData.remainProductTime - DeltaSecondes, 0.0f, stationData.maxItemListRefreshTime);
			}
		}
		for (FStructureInfo& gateData : sectorData.GateList) {
			if (gateData.isDestroyed)
				gateData.remainRespawnTime = FMath::Clamp(gateData.remainRespawnTime - DeltaSecondes, 0.0f, gateData.maxRespawnTime);
			else {
				_tempValue = DeltaSecondes / gateData.maxRespawnTime;
				gateData.structureShieldRate = FMath::Clamp(gateData.structureShieldRate + _tempValue, 0.0f, 1.0f);
				gateData.structureArmorRate = FMath::Clamp(gateData.structureArmorRate + _tempValue, 0.0f, 1.0f);
				gateData.structureHullRate = FMath::Clamp(gateData.structureHullRate + _tempValue, 0.0f, 1.0f);

				gateData.remainItemListRefreshTime = FMath::Clamp(gateData.remainItemListRefreshTime - DeltaSecondes, 0.0f, gateData.maxItemListRefreshTime);
			}
		}
	}

	//구조물 리젠 및 리셋 처리
	for (FStructureInfo& stationData : currentSectorInfo->StationList) {
		if (stationData.isDestroyed) {
			if (stationData.remainRespawnTime <= 0.0f && stationData.isRespawnable) {
				AStation* _regenStation = Cast<AStation>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(),
					FTransform(stationData.structureRotation, FVector(stationData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

				if (USafeENGINE::IsValid(_regenStation)) {
					_regenStructure = Cast<IStructureable>(_regenStation);
					_regenStructure->SetStructureData(stationData);
					UGameplayStatics::FinishSpawningActor(_regenStation, _regenStation->GetActorTransform());
				}
			}
		} else {
			if (stationData.remainItemListRefreshTime <= 0.0f) {
				_findIndex = -1;
				_addAmount = 0;
				_item = FItem();

				for (int index = 0; index < stationData.itemSellListId.Num(); index++) {
					if (stationData.itemSellListId[index].sellingChance >= FMath::RandRange(0.0, 100.0f)) {
						_item = FItem(stationData.itemSellListId[index].sellingItemID, 1);
						_findIndex = USafeENGINE::FindItemSlot(stationData.itemList, _item);
						_addAmount = FMath::RandRange(stationData.itemSellListId[index].sellingItemMinAmount, stationData.itemSellListId[index].sellingItemMaxAmount);

						if (_findIndex > -1)
							stationData.itemList[_findIndex].itemAmount += _addAmount;
						else if (_addAmount > 0)
							stationData.itemList.Emplace(FItem(stationData.itemSellListId[index].sellingItemID, _addAmount));
					} else {
						_item = FItem(stationData.itemSellListId[index].sellingItemID, 1);
						_findIndex = USafeENGINE::FindItemSlot(stationData.itemList, _item);

						if (_findIndex > -1)
							stationData.itemList.RemoveAtSwap(_findIndex);
					}
				}
				stationData.remainItemListRefreshTime = stationData.maxItemListRefreshTime;
			}
			if (stationData.remainProductTime <= 0.0f && (stationData.structureType == StructureType::ProductionFacility
				|| stationData.structureType == StructureType::Hub) && stationData.productItemList.Num() > 0) {

				_canProduct = true;
				_findIndex = -1;

				for (int index = 0; index < stationData.consumptItemList.Num(); index++) {
					_findIndex = USafeENGINE::FindItemSlot(stationData.itemList, stationData.consumptItemList[index]);

					if (_findIndex < 0 || stationData.itemList[_findIndex].itemAmount < stationData.consumptItemList[index].itemAmount) {
						_canProduct = false;
						break;
					}
				}

				if (_canProduct == true) {
					for (int index = 0; index < stationData.consumptItemList.Num(); index++) {
						_findIndex = USafeENGINE::FindItemSlot(stationData.itemList, stationData.consumptItemList[index]);
						stationData.itemList[_findIndex].itemAmount -= stationData.consumptItemList[index].itemAmount;
						if (stationData.itemList[_findIndex].itemAmount <= 0)
							stationData.itemList.RemoveAt(_findIndex);
					}

					for (int index = 0; index < stationData.productItemList.Num(); index++) {
						if (!_canProduct || index > stationData.maxProductAmount.Num() || stationData.itemList[_findIndex].itemAmount >= stationData.maxProductAmount[index])
							break;

						_findIndex = USafeENGINE::FindItemSlot(stationData.itemList, stationData.productItemList[index]);
						if (_findIndex > -1)
							stationData.itemList[_findIndex].itemAmount += stationData.productItemList[index].itemAmount;
						else
							stationData.itemList.Emplace(FItem(stationData.productItemList[index]));
					}
					stationData.remainProductTime = stationData.maxProductTime;
				}
			}
		}
	}
	for (FStructureInfo& gateData : currentSectorInfo->GateList) {
		if (gateData.isDestroyed) {
			if (gateData.remainRespawnTime <= 0.0f && gateData.isRespawnable) {
				AGate* _regenGate = Cast<AGate>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AGate::StaticClass(),
					FTransform(gateData.structureRotation, FVector(gateData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

				if (USafeENGINE::IsValid(_regenGate)) {
					_regenStructure = Cast<IStructureable>(_regenGate);
					_regenStructure->SetStructureData(gateData);
					UGameplayStatics::FinishSpawningActor(_regenGate, _regenGate->GetActorTransform());
				}
			}
		} else if (gateData.remainItemListRefreshTime <= 0.0f) {
			_findIndex = -1;
			_addAmount = 0;
			_item = FItem();

			for (int index = 0; index < gateData.itemSellListId.Num(); index++) {
				if (gateData.itemSellListId[index].sellingChance >= FMath::RandRange(0.0, 100.0f)) {
					_item = FItem(gateData.itemSellListId[index].sellingItemID, 1);
					_findIndex = USafeENGINE::FindItemSlot(gateData.itemList, _item);
					_addAmount = FMath::RandRange(gateData.itemSellListId[index].sellingItemMinAmount, gateData.itemSellListId[index].sellingItemMaxAmount);

					if (_findIndex > -1)
						gateData.itemList[_findIndex].itemAmount += _addAmount;
					else if (_addAmount > 0)
						gateData.itemList.Emplace(FItem(gateData.itemSellListId[index].sellingItemID, _addAmount));
				} else {
					_item = FItem(gateData.itemSellListId[index].sellingItemID, 1);
					_findIndex = USafeENGINE::FindItemSlot(gateData.itemList, _item);

					if (_findIndex > -1)
						gateData.itemList.RemoveAtSwap(_findIndex);
				}
			}
			gateData.remainItemListRefreshTime = gateData.maxItemListRefreshTime;
		}
	}
	Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOverTime();

	//섹터 내 함선 최대 수 체크
	if (currentShipCapacity < shipRegenLimit) {
		shipRegenAmount = FMath::RoundToInt((shipRegenLimit - objectInSector.Num()) * 0.05f);
		//리젠 시작
		for (int index = 0; index < shipRegenAmount; index++) {
			randomResultForRegen = FMath::RandRange(0, totalChanceFactor);
			//리젠 확률 가중치 계산
			for (int index1 = 0; index1 < currentSectorInfo->ShipRegenData.Num(); index1++) {

				_totalFactorInTick -= FMath::Max(currentSectorInfo->ShipRegenData[index1].regenChanceFactor, 0);
				if (_totalFactorInTick < 1) {
					//게이트로부터 출현하는 함선 생성 및 연출
					if (currentSectorInfo->ShipRegenData[index1].isPlacementToGate && currentSectorInfo->GateList.Num() > 0) {
						randomGateForRegenLocation = FMath::RandRange(0, currentSectorInfo->GateList.Num() - 1);

						_tempStationData = _tempInstance->GetStationData(currentSectorInfo->GateList[randomGateForRegenLocation].structureID);
						_tempNPCData = _tempInstance->GetNPCData(currentSectorInfo->ShipRegenData[index1].objectID);
						_tempShipData = _tempInstance->GetShipData(_tempNPCData.ShipID);

						FRotator _rotator = FRotator(0.0f, currentSectorInfo->GateList[randomGateForRegenLocation].structureRotation.Yaw, 0.0f);
						FVector _location = FVector(currentSectorInfo->GateList[randomGateForRegenLocation].structureLocation, 0.0f);
						_location += USafeENGINE::GetRandomLocation(true) * (_tempShipData.LengthToLongAsix * 0.5f
							+ _tempStationData.LengthToLongAsix * 0.5f + FMath::FRandRange(10.0f, 100.0f));

						AShip* _regenShip = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(),
							FTransform(_rotator, _location), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

						if (_regenShip != nullptr) {
							_regenShip->InitObject(currentSectorInfo->ShipRegenData[index1].objectID);
							UGameplayStatics::FinishSpawningActor(_regenShip, _regenShip->GetActorTransform());
						}
						break;
					} else if (!currentSectorInfo->ShipRegenData[index1].isPlacementToGate) {

						AShip* _regenShip = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(),
							FTransform(currentSectorInfo->ShipRegenData[index1].rotation, FVector(currentSectorInfo->ShipRegenData[index1].location, 0.0f)),
							ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

						if (_regenShip != nullptr) {
							_regenShip->InitObject(currentSectorInfo->ShipRegenData[index1].objectID);
							UGameplayStatics::FinishSpawningActor(_regenShip, _regenShip->GetActorTransform());
						}
						break;
					}

				}
			}

		}
	}
}

bool ASpaceState::SaveSpaceState(USaveLoader* saver) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FSectorData _tempData;
	FStationData _tempStationData;
	FShipData _tempShipData;

	if (saver->saveState == SaveState::NewGameCreate) {
		//All inited Sector's Data Save
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By New Game!"));

		TArray<FString> _sectorNameData;
		_tempInstance->GetAllSectorNameData(_sectorNameData);

		sectorInfo.SetNum(_sectorNameData.Num());
		for (int index = 0; index < _sectorNameData.Num(); index++) {
			_tempData = _tempInstance->GetSectorData(_sectorNameData[index]);
			sectorInfo[index] = _tempData;
		}

		saver->sectorInfo = sectorInfo;
		if (saver->sectorInfo.Num() == _sectorNameData.Num()) {
			UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] New GameSave Create Finish!"));
			return true;
		} else {
			UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] New GameSave Create Fail!"));
			return false;
		}
	} else if (saver->saveState == SaveState::UserRequest) {
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By User Request!"));
		saver->sectorInfo = sectorInfo;
		TArray<AActor*> _getAllObj;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpaceObject::StaticClass(), _getAllObj);
		TScriptInterface<IStructureable> _transStruct;

		ASpaceObject* _spaceObject;
		ACargoContainer* _cargoContainer;
		TArray<FItem> _cargoInContainer;
		AResource* _resource;

		saver->shipSaveInfo.Reserve(_getAllObj.Num());
		saver->resourceSaveInfo.Reserve(_getAllObj.Num());
		saver->containerSaveInfo.Reserve(_getAllObj.Num());
		saver->objectSaveInfo.Reserve(_getAllObj.Num());
		saver->droneSaveInfo.Reserve(_getAllObj.Num());

		int _exceptObjectCount = 0;
		//Save All Object...
		for (int index = 0; index < _getAllObj.Num(); index++) {
			if (_getAllObj[index]->IsA(APlayerShip::StaticClass())) {
				_exceptObjectCount++;
				continue;
			}
			_spaceObject = Cast<ASpaceObject>(_getAllObj[index]);
			if (!USafeENGINE::IsValid(_spaceObject))
				continue;

			switch (_spaceObject->GetObjectType()) {
			case ObjectType::Ship:
				if (_spaceObject->IsA(AShip::StaticClass()))
					saver->shipSaveInfo.Emplace(FSavedNPCShipData(_spaceObject->GetObjectID(), _spaceObject->GetFaction(), (FVector2D)_spaceObject->GetActorLocation(),
						_spaceObject->GetActorRotation(), _spaceObject->GetValue(GetStatType::currentShield), _spaceObject->GetValue(GetStatType::currentArmor),
						_spaceObject->GetValue(GetStatType::currentHull), _spaceObject->GetValue(GetStatType::currentPower)));
				break;
			case ObjectType::Drone:
				if (_spaceObject->IsA(ADrone::StaticClass()))
					saver->droneSaveInfo.Emplace(FSavedDroneData(_spaceObject->GetObjectID(), _spaceObject->GetFaction(), (FVector2D)_spaceObject->GetActorLocation(),
						_spaceObject->GetActorRotation(), _spaceObject->GetValue(GetStatType::currentShield), _spaceObject->GetValue(GetStatType::currentArmor),
						_spaceObject->GetValue(GetStatType::currentHull)));
				break;
			case ObjectType::SpaceObject:
				if (_spaceObject->GetClass() == ASpaceObject::StaticClass())
					saver->objectSaveInfo.Emplace(FSavedObjectData(_spaceObject->GetObjectID(), _spaceObject->GetFaction(), (FVector2D)_spaceObject->GetActorLocation(),
						_spaceObject->GetActorRotation(), _spaceObject->GetValue(GetStatType::currentHull)));
				break;
			case ObjectType::Container:
				if (_spaceObject->IsA(ACargoContainer::StaticClass())) {
					_cargoContainer = Cast<ACargoContainer>(_spaceObject);
					_cargoContainer->GetAllCargo(_cargoInContainer);
					saver->containerSaveInfo.Emplace(FSavedContainerData(_cargoContainer->GetObjectID(), (FVector2D)_cargoContainer->GetActorLocation(),
							_cargoContainer->GetActorRotation(), _cargoContainer->GetValue(GetStatType::currentHull), _cargoInContainer));
				}
				break;
			case ObjectType::Resource:
				if (_spaceObject->IsA(ACargoContainer::StaticClass())) {
					_resource = Cast<AResource>(_spaceObject);
					saver->resourceSaveInfo.Emplace(FSavedResourceData(_resource->GetObjectID(), (FVector2D)_resource->GetActorLocation(),
						_resource->GetActorRotation(), _resource->GetValue(GetStatType::currentHull), _resource->GetResourceAmount()));
				}
				break;
			default:
				break;
			}
		}
		saver->relation = factionRelationship;
		saver->playerFactionName = playerFactionName;
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] GameSave Create Finish!"));
		return true;
	} 
	else if (saver->saveState == SaveState::BeforeWarp) {
		//Just Save Sector Info(+ Structure Data)
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By Warp!"));
		saver->sectorInfo = sectorInfo;
		saver->relation = factionRelationship;
		saver->playerFactionName = playerFactionName;

		saver->position = FVector(0.0f, 0.0f, 0.0f);
		saver->rotation = FRotator(0.0f, 0.0f, 0.0f);
		FSectorData* _nextSectorInfo = nullptr;

		for (int index = 0; index < sectorInfo.Num(); index++) {
			if (saver->sectorInfo[index].nSectorName.ToString().Equals(saver->sectorName), ESearchCase::IgnoreCase) {
				_nextSectorInfo = &saver->sectorInfo[index];
				break;
			}
		}
		if (_nextSectorInfo != nullptr) {
			for (int index = 0; index < _nextSectorInfo->GateList.Num(); index++) {
				if (_nextSectorInfo->GateList[index].LinkedSector.Compare(UGameplayStatics::GetCurrentLevelName(GetWorld()), ESearchCase::IgnoreCase) == 0) {
					_tempStationData = _tempInstance->GetStationData(_nextSectorInfo->GateList[index].structureID);
					_tempShipData = _tempInstance->GetShipData(saver->shipID);

					saver->rotation.Yaw = _nextSectorInfo->GateList[index].structureRotation.Yaw;
					saver->position = FVector(_nextSectorInfo->GateList[index].structureLocation, 0.0f);
					saver->position += USafeENGINE::GetRandomLocation(true)
						* (_tempStationData.LengthToLongAsix * 0.5f + _tempShipData.LengthToLongAsix * 0.5f + FMath::FRandRange(10.0f, 100.0f));
					break;
				}
			}
		}
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] GameSave Create Finish!"));
		return true;
	}
	UE_LOG(LogClass, Log, TEXT("[Error][SpaceState][SaveSpaceState] GameSave Create Fail!"));
	return false;
}

bool ASpaceState::LoadSpaceState(USaveLoader* loader) {
	bool _sectorDataFind = false;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FStationData _tempStationData;
	FNPCData _tempNPCData;
	FShipData _tempShipData;
	FResourceData _tempResourceData;
	FNewStartPlayerData _tempNewStartData;

	AActor* _obj;
	AShip* _ship;
	IStructureable* _sObj;
	ADrone* _drone;
	ASpaceObject* _spaceObject;
	ACargoContainer* _container;
	AResource* _resource;
	sectorInfo = loader->sectorInfo;

	for (int index = 0; index < sectorInfo.Num(); index++) {
		if (loader->sectorInfo[index].nSectorName.ToString().Equals(UGameplayStatics::GetCurrentLevelName(GetWorld()), ESearchCase::IgnoreCase)) {
			currentSectorInfo = &sectorInfo[index];
			_sectorDataFind = true;
			shipRegenLimit = FMath::Min(300, currentSectorInfo->ShipRegenTotal);

			for (int index1 = 0; index1 < currentSectorInfo->ShipRegenData.Num(); index1++)
				totalChanceFactor += currentSectorInfo->ShipRegenData[index1].regenChanceFactor;
			break;
		}
	}
	if (_sectorDataFind == false) {
		UE_LOG(LogClass, Log, TEXT("[Error][SpaceState][LoadSpaceState] Can't Find Sector Data!"));
		return false;
	}

	if (loader->saveState == SaveState::UserRequest) {
		//Load Sector Data
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Start Load File Create By User Request!"));

		//Load Ship Data
		for (FSavedNPCShipData& shipData : loader->shipSaveInfo) {
			_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(), FTransform(FRotator(0.0f, shipData.npcShipRotation.Yaw, 0.0f),
				FVector(shipData.npcShipLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (_obj != nullptr) {
				_ship = Cast<AShip>(_obj);
				_ship->InitObject(shipData.npcShipID);
				_ship->LoadBaseObject(shipData.npcShipShield, shipData.npcShipArmor, shipData.npcShipHull, shipData.npcShipPower);
				_ship->SetFaction(shipData.npcShipFaction);
				UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
			}
		}
		//Load Structure Data
		for (FStructureInfo& structureData : currentSectorInfo->StationList) {
			if (!structureData.isDestroyed) {
				_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(), FTransform(FRotator(0.0f, structureData.structureRotation.Yaw, 0.0f),
					FVector(structureData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				if (_obj != nullptr) {
					_sObj = Cast<IStructureable>(_obj);
					_sObj->SetStructureData(structureData);
					UNavigationSystem::GetCurrent(GetWorld())->UpdateActorInNavOctree(*_obj);
					UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
				}
			}
		}
		for (FStructureInfo& structureData : currentSectorInfo->GateList) {
			if (!structureData.isDestroyed) {
				_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(), FTransform(FRotator(0.0f, structureData.structureRotation.Yaw, 0.0f),
					FVector(structureData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				if (_obj != nullptr) {
					_sObj = Cast<IStructureable>(_obj);
					_sObj->SetStructureData(structureData);
					UNavigationSystem::GetCurrent(GetWorld())->UpdateActorInNavOctree(*_obj);
					UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
				}
			}
		}
		//Load Drone Data - Player
		//Load Drone Data
		for (FSavedDroneData& droneData : loader->droneSaveInfo) {
			_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ADrone::StaticClass(), FTransform(FRotator(0.0f, droneData.droneRotation.Yaw, 0.0f),
				FVector(droneData.droneLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (_obj != nullptr) {
				_drone = Cast<ADrone>(_obj);
				_drone->InitObject(droneData.droneID);
				_drone->LoadBaseObject(droneData.droneShield, droneData.droneArmor, droneData.droneHull, 0.0f);
				_drone->SetFaction(droneData.droneFaction);
				UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
			}
		}
		//Load SpaceObject Data
		for (FSavedObjectData& objectData : loader->objectSaveInfo){
			_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ASpaceObject::StaticClass(), FTransform(FRotator(0.0f, objectData.spaceObjectRotation.Yaw, 0.0f),
				FVector(objectData.spaceObjectLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (_obj != nullptr) {
				_spaceObject = Cast<ASpaceObject>(_obj);
				_spaceObject->InitObject(objectData.spaceObjectID);
				_spaceObject->LoadBaseObject(0.0f, 0.0f, 0.0f, objectData.spaceObjectDurability);
				_spaceObject->SetFaction(objectData.spaceObjectFaction);
				UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
			}
		}
		//Load Container Data
		for (FSavedContainerData& containerData : loader->containerSaveInfo) {
			_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ACargoContainer::StaticClass(), FTransform(FRotator(0.0f, containerData.containerRotation.Yaw, 0.0f),
				FVector(containerData.containerLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (_obj != nullptr) {
				_container = Cast<ACargoContainer>(_obj);
				_container->InitObject(containerData.containerID);
				_container->LoadBaseObject(0.0f, 0.0f, 0.0f, containerData.containerDurability);
				_container->AddCargo(containerData.containerCargo);
				UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
			}
		}
		//Load Resources
		for (FSavedResourceData& resourceData : loader->resourceSaveInfo){
			_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AResource::StaticClass(), FTransform(FRotator(0.0f, resourceData.resourceRotation.Yaw, 0.0f),
				FVector(resourceData.resourceLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (_obj != nullptr) {
				_resource = Cast<AResource>(_obj);
				_resource->InitObject(resourceData.resourceID);
				_resource->SetResource(resourceData.resourceDurability, resourceData.resourceAmount);
				UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
			}
		}
	}
	//saveState == BeforeWarp or saveState == NewGameCreate
	else {
		//Just Load Sector Info(+ Structure Data)
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Start Load File Create By Warp to Next Sector!"));

		//Load Structure
		for (int index = 0; index < currentSectorInfo->StationList.Num(); index++) {
			if (!currentSectorInfo->StationList[index].isDestroyed) {
				_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(), FTransform(FRotator(0.0f, currentSectorInfo->StationList[index].structureRotation.Yaw, 0.0f),
					FVector(currentSectorInfo->StationList[index].structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				if (_obj != nullptr) {
					_sObj = Cast<IStructureable>(_obj);
					_sObj->SetStructureData(currentSectorInfo->StationList[index]);
					UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
				}
			}
		}
		for (int index = 0; index < currentSectorInfo->GateList.Num(); index++) {
			if (!currentSectorInfo->GateList[index].isDestroyed) {
				_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AGate::StaticClass(), FTransform(FRotator(0.0f, currentSectorInfo->GateList[index].structureRotation.Yaw, 0.0f),
					FVector(currentSectorInfo->GateList[index].structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				if (_obj != nullptr) {
					_sObj = Cast<IStructureable>(_obj);
					_sObj->SetStructureData(currentSectorInfo->GateList[index]);
					UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
				}
			}
		}

		//Generate Inited Ship
		for (int index = 0; index < currentSectorInfo->ShipInitedData.Num(); index++) {
			if (FMath::RandRange(0, 100) < 100 - FMath::Clamp(currentSectorInfo->ShipInitedData[index].regenChanceFactor, 0, 100))
				continue;

			_ship = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(), FTransform(currentSectorInfo->ShipInitedData[index].rotation,
				FVector(currentSectorInfo->ShipInitedData[index].location, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (_ship != nullptr) {
				_ship->InitObject(currentSectorInfo->ShipInitedData[index].objectID);
				UGameplayStatics::FinishSpawningActor(_ship, _ship->GetActorTransform());
			}
		}

		//Generate Inited Resources
		for (int index = 0; index < currentSectorInfo->ResourceInitedData.Num(); index++) {
			if (FMath::RandRange(0, 100) < 100 - FMath::Clamp(currentSectorInfo->ResourceInitedData[index].regenChanceFactor, 0, 100))
				continue;

			_resource = Cast<AResource>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AResource::StaticClass(), FTransform(currentSectorInfo->ResourceInitedData[index].rotation,
				FVector(currentSectorInfo->ResourceInitedData[index].location, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (_resource != nullptr) {
				_resource->InitObject(currentSectorInfo->ResourceInitedData[index].objectID);
				UGameplayStatics::FinishSpawningActor(_resource, _resource->GetActorTransform());
			}
		}
	}

	playerFactionName = loader->playerFactionName;
	factionRelationship = loader->relation;
	tempFactionRelationship = factionRelationship;

	UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Relationship In save file : %d"), factionRelationship.Num());
	if (factionRelationship.Num() != factionEnumPtr->NumEnums() - 4) {
		factionRelationship.SetNum(factionEnumPtr->NumEnums() - 4);
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Relationship Init not Matching with save file! Some Relationship Data Will be Damaged! Set To %d."), factionRelationship.Num());
	}
	UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Game Load Finish!"));
	return true;
}

void ASpaceState::GetCurrentSectorInfo(FSectorData& getSectorInfo) {
	getSectorInfo = *currentSectorInfo;
}

bool ASpaceState::isValidSector(const FString& checkSectorName) const {
	for (int index = 0; index < sectorInfo.Num(); index++) {
		if (sectorInfo[index].nSectorName.ToString().Equals(checkSectorName))
			return true;
	}
	return false;
}

void ASpaceState::AccumulateToShipCapacity(const bool isDestroying) {
	if (isDestroying)
		currentShipCapacity--;
	else currentShipCapacity++;

	return;
}

Peer ASpaceState::PeerIdentify(const Faction requestor, const Faction target, const bool isRealRelation) const {

	if (target == Faction::Neutral)
		return Peer::Neutral;
	if (requestor == target)
		return Peer::AllyStrong;

	Peer _result = Peer::Neutral;
	float _relation = 0.0f;

	if (requestor > Faction::PlayerFoundingFaction && target > Faction::PlayerFoundingFaction) {
		switch (requestor) {
		case Faction::PrimusEmpire:
			switch (target) {
			case Faction::ValenciaProtectorate:		_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
			break;
		case Faction::FlorenceTradeCoalition:
			switch (target) {
			case Faction::FreeCitizenFederation:	_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
			break;
		case Faction::FreeCitizenFederation:
			switch (target) {
			case Faction::FlorenceTradeCoalition:	_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
		case Faction::ValenciaProtectorate:
			switch (target) {
			case Faction::PrimusEmpire:				_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
		case Faction::HeartOfLiberty:
			switch (target) {
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
		case Faction::Pirate:
			_result = Peer::Enemy; break;
		default: break;
		}
	} 
	else {
		Faction _targetFaction;

		if ((requestor == Faction::Player || requestor == Faction::PlayerFoundingFaction) && (target == Faction::Player || target == Faction::PlayerFoundingFaction))
			_relation = relationwithPlayerEmpire;
		else {
			if (requestor > Faction::PlayerFoundingFaction)
				_targetFaction = requestor;
			else if (target > Faction::PlayerFoundingFaction)
				_targetFaction = target;
			else return _result;

			if (isRealRelation)
				_relation = FMath::Clamp(factionRelationship[FMath::Clamp((int)_targetFaction - (int)Faction::PrimusEmpire, 0, factionRelationship.Num())], _define_FactionRelationshipMIN, _define_FactionRelationshipMAX);
			else
				_relation = FMath::Clamp(tempFactionRelationship[FMath::Clamp((int)_targetFaction - (int)Faction::PrimusEmpire, 0, factionRelationship.Num())], _define_FactionRelationshipMIN, _define_FactionRelationshipMAX);
		}
		if (_relation >= _define_RelationThresholdAllyStrong)
			_result = Peer::AllyStrong;
		else if (_relation >= _define_RelationThresholdAlly && _relation < _define_RelationThresholdAllyStrong)
			_result = Peer::Ally;
		else if (_relation >= _define_RelationThresholdFriend && _relation < _define_RelationThresholdAlly)
			_result = Peer::Friendship;
		else if (_relation <= _define_RelationThresholdBoundary && _relation > _define_RelationThresholdEnemy)
			_result = Peer::Boundary;
		else if (_relation <= _define_RelationThresholdEnemy && _relation > _define_RelationThresholdEnemyStrong)
			_result = Peer::Enemy;
		else if (_relation <= _define_RelationThresholdEnemyStrong)
			_result = Peer::EnemyStrong;
	}
	return _result;
}

float ASpaceState::GetRelationshipArray(TArray<float>& relationshipArray, bool isRealRelation) const {

	if (isRealRelation)
		relationshipArray = factionRelationship;
	else
		relationshipArray = tempFactionRelationship;
	return relationwithPlayerEmpire;
}

void ASpaceState::ApplyRelation(const Faction targetFaction, float damageForConvertRelation) {

	damageForConvertRelation = FMath::Clamp(damageForConvertRelation * FMath::FRandRange(_define_DamagetoRelationFactorMIN, _define_DamagetoRelationFactorMAX),
		_define_LimitApplyRelationPerOnceMIN, _define_LimitApplyRelationPerOnceMAX);

	int _tempIndex = FMath::Min3(tempFactionRelationship.Num(), (int)targetFaction, 0);
	tempFactionRelationship[_tempIndex] = FMath::Clamp(tempFactionRelationship[_tempIndex] - damageForConvertRelation,
		_define_FactionRelationshipMIN, _define_FactionRelationshipMAX);
	return;
}

void ASpaceState::ApplyRelation(const Faction targetFaction, float SPForConvertRelation, const bool isRealRelation) {

	SPForConvertRelation = FMath::Clamp(SPForConvertRelation * FMath::FRandRange(_define_SPtoRelationFactorMIN, _define_SPtoRelationFactorMAX),
		_define_LimitApplyRelationPerOnceMIN, _define_LimitApplyRelationPerOnceMAX);

	int _tempIndex = 0;
	if (isRealRelation) {
		_tempIndex = FMath::Min3(factionRelationship.Num(), (int)targetFaction, 0);
		factionRelationship[_tempIndex] = FMath::Clamp(factionRelationship[_tempIndex] - SPForConvertRelation,
			_define_FactionRelationshipMIN, _define_FactionRelationshipMAX);
	}
	else {
		_tempIndex = FMath::Min3(tempFactionRelationship.Num(), (int)targetFaction, 0);
		tempFactionRelationship[_tempIndex] = FMath::Clamp(tempFactionRelationship[_tempIndex] - SPForConvertRelation,
			_define_FactionRelationshipMIN, _define_FactionRelationshipMAX);
	}
		
	return;
}