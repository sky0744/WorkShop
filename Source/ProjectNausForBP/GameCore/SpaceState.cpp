// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceState.h"


ASpaceState::ASpaceState() {

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 60.0f;
}

void ASpaceState::BeginPlay() {
	Super::BeginPlay();

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

				if (_regenStation != nullptr) {
					_regenStructure = Cast<IStructureable>(_regenStation);
					_regenStructure->SetStructureData(stationData);
					UGameplayStatics::FinishSpawningActor(_regenStation, _regenStation->GetActorTransform());
					_regenStation->AddActorWorldOffset(USafeENGINE::GetRandomLocation(true) * 0.01f);
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

				if (_regenGate != nullptr) {
					_regenStructure = Cast<IStructureable>(_regenGate);
					_regenStructure->SetStructureData(gateData);
					UGameplayStatics::FinishSpawningActor(_regenGate, _regenGate->GetActorTransform());
					_regenGate->AddActorWorldOffset(USafeENGINE::GetRandomLocation(true) * 0.01f);
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
						_location += USafeENGINE::GetRandomLocation(true) * (_tempShipData.lengthToLongAsix * 0.5f
							+ _tempStationData.lengthToLongAsix * 0.5f + FMath::FRandRange(10.0f, 100.0f));

						AShip* _regenShip = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(),
							FTransform(_rotator, _location), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

						if (_regenShip != nullptr) {
							_regenShip->InitObject(currentSectorInfo->ShipRegenData[index1].objectID);
							UGameplayStatics::FinishSpawningActor(_regenShip, _regenShip->GetActorTransform());
							_regenShip->AddActorWorldOffset(USafeENGINE::GetRandomLocation(true) * 0.01f);
						}
						break;
					} else if (!currentSectorInfo->ShipRegenData[index1].isPlacementToGate) {

						AShip* _regenShip = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(),
							FTransform(currentSectorInfo->ShipRegenData[index1].rotation, FVector(currentSectorInfo->ShipRegenData[index1].location, 0.0f)),
							ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

						if (_regenShip != nullptr) {
							_regenShip->InitObject(currentSectorInfo->ShipRegenData[index1].objectID);
							UGameplayStatics::FinishSpawningActor(_regenShip, _regenShip->GetActorTransform());
							_regenShip->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
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
		}
		else {
			UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] New GameSave Create Fail!"));
			return false;
		}
	}
	else if (saver->saveState == SaveState::UserRequest) {
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By User Request!"));
		saver->sectorInfo = sectorInfo;
		TArray<AActor*> _getAllObj;
		ASpaceObject* _obj;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpaceObject::StaticClass(), _getAllObj);

		TScriptInterface<IStructureable> _transStruct;
		AResource* _resource;

		saver->npcShipID.Reserve(_getAllObj.Num() - 1);
		saver->npcShipLocation.Reserve(_getAllObj.Num() - 1);
		saver->npcShipRotation.Reserve(_getAllObj.Num() - 1);
		saver->npcShipShield.Reserve(_getAllObj.Num() - 1);
		saver->npcShipArmor.Reserve(_getAllObj.Num() - 1);
		saver->npcShipHull.Reserve(_getAllObj.Num() - 1);
		saver->npcShipPower.Reserve(_getAllObj.Num() - 1);

		int _exceptObjectCount = 0;
		//Save All Object...
		for (int index = 0; index < _getAllObj.Num(); index++) {
			if (_getAllObj[index]->IsA(APlayerShip::StaticClass())) {
				_exceptObjectCount++;
				continue;
			}
			_obj = Cast<ASpaceObject>(_getAllObj[index]);

			switch (_obj->GetObjectType()) {
			case ObjectType::Ship:
				if (_obj != UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
					saver->npcShipID.Emplace(_obj->GetObjectID());
					saver->npcShipLocation.Emplace(_obj->GetActorLocation());
					saver->npcShipRotation.Emplace(_obj->GetActorRotation());
					saver->npcShipShield.Emplace(_obj->GetValue(GetStatType::currentShield));
					saver->npcShipArmor.Emplace(_obj->GetValue(GetStatType::currentArmor));
					saver->npcShipHull.Emplace(_obj->GetValue(GetStatType::currentHull));
					saver->npcShipPower.Emplace(_obj->GetValue(GetStatType::currentPower));
				}
				break;
			case ObjectType::Resource:
				_resource = Cast<AResource>(_obj);

				saver->resourceID.Emplace(_resource->GetObjectID());
				saver->resourceLocation.Emplace(_resource->GetActorLocation());
				saver->resourceRotation.Emplace(_resource->GetActorRotation());
				saver->resourceDurability.Emplace(_resource->GetResourceDurability());
				saver->resourceAmount.Emplace(_resource->GetResourceAmount());
				break;
			case ObjectType::Gate:
			case ObjectType::Station:
				break;
			default:
				break;
			}
		}
		saver->relation = factionRelation;
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] GameSave Create Finish!"));
		return true;
	}
	else if (saver->saveState == SaveState::BeforeWarp) {
		//Just Save Sector Info(+ Structure Data)
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By Warp!"));
		saver->sectorInfo = sectorInfo;
		saver->relation = factionRelation;

		saver->position = FVector(0.0f, 0.0f, 0.0f);
		saver->rotation = FRotator(0.0f, 0.0f, 0.0f);
		FSectorData* _nextSectorInfo = nullptr;

		for (int index = 0; index < sectorInfo.Num(); index++) {
			if (saver->sectorInfo[index].nSectorName.ToString().Equals(saver->sectorName), ESearchCase::IgnoreCase)
			{
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
						* (_tempStationData.lengthToLongAsix * 0.5f + _tempShipData.lengthToLongAsix * 0.5f + FMath::FRandRange(10.0f, 100.0f));
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

	AActor* _obj;
	AShip* _ship;
	AResource* _resource;
	IStructureable* _sObj;
	sectorInfo = loader->sectorInfo;

	UEnum* _EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("Faction"), true);
	factionRelation = loader->relation;
	if (factionRelation.Num() != _EnumPtr->NumEnums())
		factionRelation.SetNum(_EnumPtr->NumEnums());

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

		//Load Structure
		for (int index = 0; index < currentSectorInfo->StationList.Num(); index++) {
			if (!currentSectorInfo->StationList[index].isDestroyed) {
				_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(), FTransform(FRotator(0.0f, currentSectorInfo->StationList[index].structureRotation.Yaw, 0.0f),
					FVector(currentSectorInfo->StationList[index].structureLocation, -1.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				if (_obj != nullptr) {
					_sObj = Cast<IStructureable>(_obj);
					_sObj->SetStructureData(currentSectorInfo->StationList[index]);
					UNavigationSystem::GetCurrent(GetWorld())->UpdateActorInNavOctree(*_obj);
					UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
				}
				_obj->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
			}
		}
		for (int index = 0; index < currentSectorInfo->GateList.Num(); index++) {
			if (!currentSectorInfo->GateList[index].isDestroyed) {
				_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AGate::StaticClass(), FTransform(FRotator(0.0f, currentSectorInfo->GateList[index].structureRotation.Yaw, 0.0f),
					FVector(currentSectorInfo->GateList[index].structureLocation, -1.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				if (_obj != nullptr) {
					_sObj = Cast<IStructureable>(_obj);
					_sObj->SetStructureData(currentSectorInfo->GateList[index]);
					UNavigationSystem::GetCurrent(GetWorld())->UpdateActorInNavOctree(*_obj);
					UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
				}
				_obj->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
			}
		}

		//Load Objects
		for (int index = 0; index < loader->npcShipID.Num(); index++) {

			_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(), FTransform(FRotator(loader->npcShipRotation[index]),
				FVector((FVector2D)(loader->npcShipLocation[index]), -1.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (_obj != nullptr) {
				Cast<ASpaceObject>(_obj)->InitObject(loader->npcShipID[index]);
				Cast<ASpaceObject>(_obj)->LoadBaseObject(loader->npcShipShield[index], loader->npcShipArmor[index], loader->npcShipHull[index], loader->npcShipPower[index]);
				UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
			}
			_obj->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
		}

		//Load Resources
		for (int index = 0; index < loader->resourceID.Num(); index++) {

			_obj = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AResource::StaticClass(), FTransform(FRotator(loader->resourceRotation[index]),
				FVector((FVector2D)(loader->resourceLocation[index]), -1.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

			if (_obj != nullptr) {
				Cast<ASpaceObject>(_obj)->InitObject(loader->resourceID[index]);
				_resource = Cast<AResource>(_obj);
				_resource->SetResource(loader->resourceDurability[index], loader->resourceAmount[index]);
				UGameplayStatics::FinishSpawningActor(_obj, _obj->GetActorTransform());
			}
			_obj->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
		}
		
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Game Load Finish!"));
		return true;
	}
	else if (loader->saveState == SaveState::BeforeWarp || loader->saveState == SaveState::NewGameCreate)
	{
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
					_obj->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
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
					_obj->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
				}
			}
		}

		//Generate Inited Ship
		for (int index = 0; index < currentSectorInfo->ShipInitedData.Num(); index++) {
			_ship = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(), FTransform(currentSectorInfo->ShipInitedData[index].rotation,
				FVector(currentSectorInfo->ShipInitedData[index].location, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (_ship != nullptr) {
				_ship->InitObject(currentSectorInfo->ShipInitedData[index].objectID);
				UGameplayStatics::FinishSpawningActor(_ship, _ship->GetActorTransform());
				_ship->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
			} 
		}

		//Generate Inited Resources
		for (int index = 0; index < currentSectorInfo->ResourceInitedData.Num(); index++) {

			_resource = Cast<AResource>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AResource::StaticClass(), FTransform(currentSectorInfo->ResourceInitedData[index].rotation,
				FVector(currentSectorInfo->ResourceInitedData[index].location, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (_resource != nullptr) {
				_resource->InitObject(currentSectorInfo->ResourceInitedData[index].objectID);
				UGameplayStatics::FinishSpawningActor(_resource, _resource->GetActorTransform());
				_resource->AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
			}
		}
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Game Load Finish!"));
		return true;
	}

	UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Load File Fail!"));
	return false;
}

Peer ASpaceState::PeerIdentify(Faction requestor, Faction target) {
	Peer _result = Peer::Neutral;
	float _relation;

	if (target == Faction::Neutral)
		return Peer::Neutral;

	if (requestor == target)
		return Peer::AllyStrong;

	if (target == Faction::Pirate || requestor == Faction::Pirate) {
		return Peer::EnemyStrong;
	}

	if (target == Faction::Player) {
		_relation = factionRelation[(uint8)(requestor)];
		if (_relation >= 50.0f)
			return Peer::AllyStrong;
		else if (_relation >= 50.0f && _relation < 80.0f)
			return Peer::Ally;
		else if (_relation >= 20.0f && _relation < 50.0f)
			return Peer::Friendship;
		else if (_relation <= -20.0f && _relation > -50.0f)
			return Peer::Boundary;
		else if (_relation <= -50.0f && _relation > -80.0f)
			return Peer::Enemy;
		else if (_relation <= -80.0f)
			return Peer::EnemyStrong;
		else return Peer::Neutral;
	}
	else {
		_relation = factionRelation[(uint8)(target)];
		switch (requestor) {
		case Faction::Player:
			if (_relation >= 50.0f)
				_result = Peer::AllyStrong;
			else if (_relation >= 50.0f && _relation < 80.0f)
				_result = Peer::Ally;
			else if (_relation >= 20.0f && _relation < 50.0f)
				_result = Peer::Friendship;
			else if (_relation <= -20.0f && _relation > -50.0f)
				_result = Peer::Boundary;
			else if (_relation <= -50.0f && _relation > -80.0f)
				_result = Peer::Enemy;
			else if (_relation <= -80.0f)
				_result = Peer::EnemyStrong;
			else _result = Peer::Neutral;
			break;

		case Faction::PrimusEmpire:
			if (target == Faction::ValenciaProtectorate)
				_result = Peer::Ally;
			else _result = Peer::Enemy;
			break;
		case Faction::FlorenceTradeCoalition:
			if (target == Faction::FreeCitizenFederation)
				_result = Peer::Ally;
			else _result = Peer::Enemy;
			break;
		case Faction::FreeCitizenFederation:
			if (target == Faction::FlorenceTradeCoalition)
				_result = Peer::Ally;
			else _result = Peer::Enemy;
			break;
		case Faction::ValenciaProtectorate:
			if (target == Faction::PrimusEmpire)
				_result = Peer::Ally;
			else _result = Peer::Enemy;
			break;
		default:
			_result = Peer::TempHold;
			break;
		}
	}
	return _result;
}

void ASpaceState::GetCurrentSectorInfo(FSectorData& getSectorInfo) {
	getSectorInfo = *currentSectorInfo;
}

bool ASpaceState::isValidSector(FString& checkSectorName) {
	for (int index = 0; index < sectorInfo.Num(); index++) {
		if (sectorInfo[index].nSectorName.ToString().Equals(checkSectorName))
			return true;
	}
	return false;
}

void ASpaceState::AccumulateToShipCapacity(bool isDestroying) {
	if (isDestroying)
		currentShipCapacity--;
	else currentShipCapacity++;

	return;
}