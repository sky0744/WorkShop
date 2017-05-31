// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceState.h"


ASpaceState::ASpaceState() {
	ev_BGMComponent = CreateDefaultSubobject <UAudioComponent>(TEXT("BGMComponent"));
	ev_BGMComponent->bAutoActivate = false;
	ev_BGMComponent->OnAudioFinished.AddDynamic(this, &ASpaceState::OnBGMSettingAndPlay);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = _define_SectorRefreshTime;

	sectorInfo = TArray<FSectorData>();
	tempFactionRelationship = TArray<float>();
	factionRelationship = TArray<float>();
}

void ASpaceState::BeginPlay() {
	Super::BeginPlay();

	//GC 즉시 실행
	GetWorld()->ForceGarbageCollection(true);
	factionEnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("Faction"), true);
	currentShipCapacity = 0;

	if (UGameplayStatics::GetCurrentLevelName(GetWorld()).Equals("MainTitle", ESearchCase::IgnoreCase)) {
		ev_BGMCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, TEXT("SoundCue'/Game/Resource/Sound/SoundAsset/SoundCue/MainTitle/MainTitle_Cue.MainTitle_Cue'")));
		if (ev_BGMComponent->IsValidLowLevelFast() && ev_BGMCue->IsValidLowLevelFast()) {
			ev_BGMComponent->SetSound(ev_BGMCue);
			ev_BGMComponent->Play();
		}
	}
}

void ASpaceState::Tick(float DeltaSecondes) {
	Super::Tick(DeltaSecondes);

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (currentSectorInfo == nullptr || !IsValid(_tempInstance))
		return;
	ASpaceHUDBase* _tempHUDBase = Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	FStationData _tempStationData;
	FNPCData _tempNPCData;
	FShipData _tempShipData;
	FItem _item = FItem();

	TArray<AActor*> objectInSector;
	float _respawnFactor;
	float _structureRegenerateFactor;
	int _shipRegenAmount;
	int _randomGateIndex;
	int _currentChanceFactor = totalChanceFactor;

	//게임 내의 모든 스테이션에 대해 스테이터스 및 파괴 쿨타임, 생산 상태를 갱신
	for (FSectorData& sectorData : sectorInfo) {
		for (FStructureInfo& stationData : sectorData.StationInSector) {
			if (stationData.isDestroyed)
				stationData.remainRespawnTime = FMath::Clamp(stationData.remainRespawnTime - DeltaSecondes, -1.0f, stationData.maxRespawnTime);
			else {
				_respawnFactor = DeltaSecondes / stationData.maxRespawnTime;
				stationData.structureShieldRate = FMath::Clamp(stationData.structureShieldRate + _respawnFactor, 0.0f, 1.0f);
				stationData.structureArmorRate = FMath::Clamp(stationData.structureArmorRate + _respawnFactor, 0.0f, 1.0f);
				stationData.structureHullRate = FMath::Clamp(stationData.structureHullRate + _respawnFactor, 0.0f, 1.0f);
				stationData.remainSellingItemRefreshTime = FMath::Clamp(stationData.remainSellingItemRefreshTime - DeltaSecondes, -1.0f, stationData.maxSellingItemRefreshTime);

				if (stationData.structureType == StructureType::ProductionFacility || stationData.structureType == StructureType::Hub) {
					for (int index = 0; index < stationData.itemsInProduction.Num(); index++) {
						if (stationData.itemsInProduction[index].productTime > 0.0f) {
							stationData.itemsInProduction[index].productTime 
								= FMath::Clamp(stationData.itemsInProduction[index].productTime - DeltaSecondes, -1.0f, stationData.itemsInProduction[index].maxProductTime);
							continue;
						}
						stationData.playerItemSlot.Emplace(stationData.itemsInProduction[index].productItem.itemID, stationData.itemsInProduction[index].productItem.itemAmount);
						stationData.itemsInProduction.RemoveAtSwap(index);
					}
				}
			}
		}
		for (FStructureInfo& gateData : sectorData.GateInSector) {
			if (gateData.isDestroyed)
				gateData.remainRespawnTime = FMath::Clamp(gateData.remainRespawnTime - DeltaSecondes, -1.0f, gateData.maxRespawnTime);
			else {
				_structureRegenerateFactor = DeltaSecondes / gateData.maxRespawnTime;
				gateData.structureShieldRate = FMath::Clamp(gateData.structureShieldRate + _structureRegenerateFactor, 0.0f, 1.0f);
				gateData.structureArmorRate = FMath::Clamp(gateData.structureArmorRate + _structureRegenerateFactor, 0.0f, 1.0f);
				gateData.structureHullRate = FMath::Clamp(gateData.structureHullRate + _structureRegenerateFactor, 0.0f, 1.0f);
				gateData.remainSellingItemRefreshTime = FMath::Clamp(gateData.remainSellingItemRefreshTime - DeltaSecondes, -1.0f, gateData.maxSellingItemRefreshTime);
			}
		}
	}

	//현재 섹터 내의 스테이션에 대해 리젠, 아이템 목록을 갱신
	for (FStructureInfo& stationData : currentSectorInfo->StationInSector) {
		//리젠 갱신
		if (stationData.isDestroyed && stationData.isRespawnable && stationData.remainRespawnTime < 0.0f) {
			AStation* _regenStation = Cast<AStation>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(),
				FTransform(FRotator(), FVector(stationData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

			if (IsValid(_regenStation) && _regenStation->StaticClass()->ImplementsInterface(UStructureable::StaticClass())) {
				Cast<IStructureable>(_regenStation)->SetStructureData(stationData);
				UGameplayStatics::FinishSpawningActor(_regenStation, _regenStation->GetActorTransform());
			} else
				_regenStation->Destroy();
		}
		//아이템 목록 갱신
		if (!stationData.isDestroyed && stationData.remainSellingItemRefreshTime < 0.0f) {
			stationData.itemSlot.Empty();
			stationData.itemSlot.Reserve(stationData.itemSellArrayPackage.Num());
			for (FItemSellData& sellData : stationData.itemSellArrayPackage) {
				if (sellData.sellingChance >= FMath::RandRange(_define_ItemRefreshChanceMIN, _define_ItemRefreshChanceMAX)) 
					stationData.itemSlot.Emplace(sellData.sellingItemID, FMath::RandRange(sellData.sellingItemMinAmount, sellData.sellingItemMaxAmount));
			}
			stationData.remainSellingItemRefreshTime = stationData.maxSellingItemRefreshTime;
		}
	}
	for (FStructureInfo& gateData : currentSectorInfo->GateInSector) {
		//리젠 갱신
		if (gateData.isDestroyed && gateData.isRespawnable && gateData.remainRespawnTime < 0.0f) {
			AGate* _regenGate = Cast<AGate>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(),
				FTransform(FRotator(), FVector(gateData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

			if (IsValid(_regenGate) && _regenGate->StaticClass()->ImplementsInterface(UStructureable::StaticClass())) {
				Cast<IStructureable>(_regenGate)->SetStructureData(gateData);
				UGameplayStatics::FinishSpawningActor(_regenGate, _regenGate->GetActorTransform());
			} else
				_regenGate->Destroy();
		}
		//아이템 목록 갱신
		if (!gateData.isDestroyed && gateData.remainSellingItemRefreshTime < 0.0f) {
			gateData.itemSlot.Empty();
			gateData.itemSlot.Reserve(gateData.itemSellArrayPackage.Num());
			for (FItemSellData& sellData : gateData.itemSellArrayPackage) 
				if (sellData.sellingChance >= FMath::RandRange(_define_ItemRefreshChanceMIN, _define_ItemRefreshChanceMAX)) {
					gateData.itemSlot.Emplace(sellData.sellingItemID, FMath::RandRange(sellData.sellingItemMinAmount, sellData.sellingItemMaxAmount));
			}
			gateData.remainSellingItemRefreshTime = gateData.maxSellingItemRefreshTime;
		}
	}
	
	if (IsValid(_tempHUDBase)) {
		_tempHUDBase->UpdateUI(UpdateUIType::Dock_Cargo);
		_tempHUDBase->UpdateUI(UpdateUIType::Dock_Trade);
		_tempHUDBase->UpdateUI(UpdateUIType::Dock_Industry);
	}
	//섹터 내 함선 최대 수 체크
	if (currentShipCapacity > shipRegenLimit)
		return;
	//리젠 시작
	_shipRegenAmount = FMath::RoundToInt((shipRegenLimit - currentShipCapacity) * _define_RegenShipFactor);
	for (int index = 0; index < _shipRegenAmount; index++) {
		//리젠 확률 가중치 계산, 리젠 데이터를 거치면서 가중치를 감산, 가중치가 0에 도달한 데이터로 리젠 시도.
		_currentChanceFactor = FMath::RandRange(0, totalChanceFactor);
		for (FObjectPlacement& respawnData : currentSectorInfo->ShipRegenData) {
			_currentChanceFactor -= FMath::Max(respawnData.regenChanceFactor, 0);
			if (_currentChanceFactor > 1)
				continue;

			//게이트로부터 출현하는 함선 생성 및 연출
			if (respawnData.isPlacementToGate && currentSectorInfo->GateInSector.Num() > 0) {
				_randomGateIndex = FMath::RandRange(0, currentSectorInfo->GateInSector.Num() - 1);

				AShip* _regenShip = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(), 
					FTransform(FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f), FVector(currentSectorInfo->GateInSector[_randomGateIndex].structureLocation, 0.0f)), 
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

				if (IsValid(_regenShip)) {
					_regenShip->InitObject(respawnData.objectID);
					UGameplayStatics::FinishSpawningActor(_regenShip, _regenShip->GetActorTransform());
				}
				else
					_regenShip->Destroy();
				break;
			} 
			//고정 위치에서 출현하는 함선 생성 및 연출
			else {
				AShip* _regenShip = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(),
					FTransform(FRotator(0.0f, respawnData.rotation.Yaw, 0.0f), FVector(respawnData.location, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));

				if (IsValid(_regenShip)) {
					_regenShip->InitObject(respawnData.objectID);
					UGameplayStatics::FinishSpawningActor(_regenShip, _regenShip->GetActorTransform());
				}
				else
					_regenShip->Destroy();
				break;
			}
		}
	}
}

#pragma region Save/Load
bool ASpaceState::SaveSpaceState(USaveLoader* saver) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!IsValid(_tempInstance))
		return false;

	FSectorData _tempData;
	FStationData _tempStationData;
	FShipData _tempShipData;

	if (saver->saveState == SaveState::NewGameCreate) {
		//모든 섹터의 데이터를 초기화하고 저장. 섹터 데이터 이외의 초기 데이터는 UserState에서 처리
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By New Game!"));

		TArray<FString> _sectorNameData;
		_tempInstance->GetAllSectorNameData(_sectorNameData);

		sectorInfo.SetNum(_sectorNameData.Num());
		for (int index = 0; index < _sectorNameData.Num(); index++) {
			_tempData = _tempInstance->GetSectorData(_sectorNameData[index]);
			sectorInfo[index] = _tempData;
		}

		saver->sectorInfo = sectorInfo;
		if (saver->sectorInfo.Num() == _sectorNameData.Num())
			return true;
		else
			return false;
	}
	else if (saver->saveState == SaveState::UserRequest) {
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By User Request!"));
		saver->sectorInfo = sectorInfo;
		TArray<AActor*> _getAllObj;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpaceObject::StaticClass(), _getAllObj);
		TScriptInterface<IStructureable> _transStruct;

		ASpaceObject* _spaceObject;
		ACargoContainer* _cargoContainer;
		AResource* _resource;

		saver->shipSaveInfo.Reserve(_getAllObj.Num());
		saver->resourceSaveInfo.Reserve(_getAllObj.Num());
		saver->containerSaveInfo.Reserve(_getAllObj.Num());
		saver->objectSaveInfo.Reserve(_getAllObj.Num());
		saver->droneSaveInfo.Reserve(_getAllObj.Num());
		//함선, 자원, 컨테이너, 일반 오브젝트, 드론 등의 오브젝트를 데이터화하고 저장.
		for (int index = 0; index < _getAllObj.Num(); index++) {
			if (_getAllObj[index]->IsA(APlayerShip::StaticClass())) 
				continue;
			_spaceObject = Cast<ASpaceObject>(_getAllObj[index]);
			if (!IsValid(_spaceObject))
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
					saver->containerSaveInfo.Emplace(FSavedContainerData(_cargoContainer->GetObjectID(), (FVector2D)_cargoContainer->GetActorLocation(),
							_cargoContainer->GetActorRotation(), _cargoContainer->GetValue(GetStatType::currentHull), _cargoContainer->GetCargo()));
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
		saver->shipSaveInfo.Shrink();
		saver->resourceSaveInfo.Shrink();
		saver->containerSaveInfo.Shrink();
		saver->objectSaveInfo.Shrink();
		saver->droneSaveInfo.Shrink();

		saver->relation = factionRelationship;
		saver->relationwithPlayerEmpire = relationwithPlayerEmpire;
		saver->playerFactionName = playerFactionName;
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] GameSave Create Finish!"));
		return true;
	} 
	else if (saver->saveState == SaveState::BeforeWarp) {
		//섹터 정보 및 플레이어 시작 위치만 지정
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][SaveSpaceState] Start Save File Create By Warp!"));
		saver->sectorInfo = sectorInfo;
		saver->relation = factionRelationship;
		saver->relationwithPlayerEmpire = relationwithPlayerEmpire;
		saver->playerFactionName = playerFactionName;

		saver->position = FVector2D(0.0f, 0.0f);
		saver->rotation = FRotator(0.0f, 0.0f, 0.0f);
		FSectorData* _nextSectorInfo = nullptr;

		for (int index = 0; index < sectorInfo.Num(); index++) {
			if (saver->sectorInfo[index].nSectorName.ToString().Equals(saver->sectorName), ESearchCase::IgnoreCase) {
				_nextSectorInfo = &saver->sectorInfo[index];
				break;
			}
		}
		//플레이어의 시작 위치를 지정
		//PlayerPawn은 Manager급 액터들과 함께 일반 액터에 비해 빨리 스폰. 일반 액터인 AGate는 일반적으로 AlwaysSpawn으로 스폰되므로 겹치게 됨.
		//따라서, 점프 이전에 미리 좌표를 계산해서 지정해야 겹치지 않고 스폰될 수 있음.
		if (_nextSectorInfo != nullptr) {
			for (int index = 0; index < _nextSectorInfo->GateInSector.Num(); index++) {
				if (_nextSectorInfo->GateInSector[index].LinkedSector.Compare(UGameplayStatics::GetCurrentLevelName(GetWorld()), ESearchCase::IgnoreCase) == 0) {
					_tempStationData = _tempInstance->GetStationData(_nextSectorInfo->GateInSector[index].structureID);
					_tempShipData = _tempInstance->GetShipData(saver->shipID);

					saver->rotation.Yaw = FMath::FRandRange(0.0f, 360.0f);
					saver->position = _nextSectorInfo->GateInSector[index].structureLocation;
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

	AShip* _ship;
	AStation* _station;
	AGate* _gate;
	ADrone* _drone;
	ASpaceObject* _spaceObject;
	ACargoContainer* _container;
	AResource* _resource;
	IStructureable* _sObj;
	sectorInfo = loader->sectorInfo;

	for (FSectorData& sectorData : sectorInfo) {
		if (sectorData.nSectorName.ToString().Equals(UGameplayStatics::GetCurrentLevelName(GetWorld()), ESearchCase::IgnoreCase)) {
			currentSectorInfo = &sectorData;
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
			_ship = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(), FTransform(FRotator(0.0f, shipData.npcShipRotation.Yaw, 0.0f),
				FVector(shipData.npcShipLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (IsValid(_ship)) {
				_ship->InitObject(shipData.npcShipID);
				_ship->LoadBaseObject(shipData.npcShipShield, shipData.npcShipArmor, shipData.npcShipHull, shipData.npcShipPower);
				_ship->SetFaction(shipData.npcShipFaction);
				UGameplayStatics::FinishSpawningActor(_ship, _ship->GetActorTransform());
			} else if(_ship)
				_ship->Destroy();
		}
		//Load Structure Data
		for (FStructureInfo& structureData : currentSectorInfo->StationInSector) {
			if (!structureData.isDestroyed) {
				_station = Cast<AStation>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(), FTransform(FRotator(),
					FVector(structureData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
				if (IsValid(_station) && _station->StaticClass()->ImplementsInterface(UStructureable::StaticClass())) {
					_sObj = Cast<IStructureable>(_station);
					_sObj->SetStructureData(structureData);
					UGameplayStatics::FinishSpawningActor(_station, _station->GetActorTransform());
				} else if (_station)
					_station->Destroy();
			}
		}
		for (FStructureInfo& structureData : currentSectorInfo->GateInSector) {
			if (!structureData.isDestroyed) {
				_gate = Cast<AGate>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AGate::StaticClass(), FTransform(FRotator(),
					FVector(structureData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
				if (IsValid(_gate) && _gate->StaticClass()->ImplementsInterface(UStructureable::StaticClass())) {
					_sObj = Cast<IStructureable>(_gate);
					_sObj->SetStructureData(structureData);
					UGameplayStatics::FinishSpawningActor(_gate, _gate->GetActorTransform());
				} else if (_gate)
					_gate->Destroy();
			}
		}
		//Load Drone Data - Player
		//Load Drone Data
		for (FSavedDroneData& droneData : loader->droneSaveInfo) {
			_drone = Cast<ADrone>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ADrone::StaticClass(), FTransform(FRotator(0.0f, droneData.droneRotation.Yaw, 0.0f),
				FVector(droneData.droneLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (IsValid(_drone)) {
				_drone->InitObject(droneData.droneID);
				_drone->LoadBaseObject(droneData.droneShield, droneData.droneArmor, droneData.droneHull, 0.0f);
				_drone->SetFaction(droneData.droneFaction);
				UGameplayStatics::FinishSpawningActor(_drone, _drone->GetActorTransform());
			} else if(_drone)
				_drone->Destroy();
		}
		//Load SpaceObject Data
		for (FSavedObjectData& objectData : loader->objectSaveInfo){
			_spaceObject = Cast<ASpaceObject>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ASpaceObject::StaticClass(), FTransform(FRotator(0.0f, objectData.spaceObjectRotation.Yaw, 0.0f),
				FVector(objectData.spaceObjectLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (IsValid(_spaceObject)) {
				_spaceObject->InitObject(objectData.spaceObjectID);
				_spaceObject->LoadBaseObject(0.0f, 0.0f, 0.0f, objectData.spaceObjectDurability);
				_spaceObject->SetFaction(objectData.spaceObjectFaction);
				UGameplayStatics::FinishSpawningActor(_spaceObject, _spaceObject->GetActorTransform());
			} else if(_spaceObject)
				_spaceObject->Destroy();
		}
		//Load Container Data
		for (FSavedContainerData& containerData : loader->containerSaveInfo) {
			_container = Cast<ACargoContainer>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ACargoContainer::StaticClass(), FTransform(FRotator(0.0f, containerData.containerRotation.Yaw, 0.0f),
				FVector(containerData.containerLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (IsValid(_container)) {
				_container->InitObject(containerData.containerID);
				_container->LoadBaseObject(0.0f, 0.0f, 0.0f, containerData.containerDurability);
				_container->SetCargo(containerData.containerCargo);
				UGameplayStatics::FinishSpawningActor(_container, _container->GetActorTransform());
			} else if(_container)
				_container->Destroy();
		}
		//Load Resources
		for (FSavedResourceData& resourceData : loader->resourceSaveInfo){
			_resource = Cast<AResource>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AResource::StaticClass(), FTransform(FRotator(0.0f, resourceData.resourceRotation.Yaw, 0.0f),
				FVector(resourceData.resourceLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (IsValid(_resource)) {
				_resource->InitObject(resourceData.resourceID);
				_resource->SetResource(resourceData.resourceDurability, resourceData.resourceAmount);
				UGameplayStatics::FinishSpawningActor(_resource, _resource->GetActorTransform());
			} else if(_resource)
				_resource->Destroy();
		}
	}
	else {
		//Just Load Sector Info(+ Structure Data)
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Start Load File Create By Warp to Next Sector!"));

		//Load Structure
		for (FStructureInfo& structureData : currentSectorInfo->StationInSector) {
			if (!structureData.isDestroyed) {
				_station = Cast<AStation>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AStation::StaticClass(), FTransform(FRotator(),
					FVector(structureData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
				if (IsValid(_station) && _station->StaticClass()->ImplementsInterface(UStructureable::StaticClass())) {
					_sObj = Cast<IStructureable>(_station);
					_sObj->SetStructureData(structureData);
					UGameplayStatics::FinishSpawningActor(_station, _station->GetActorTransform());
				} else if(_station)
					_station->Destroy();
			}
		}
		for (FStructureInfo& structureData : currentSectorInfo->GateInSector) {
			if (!structureData.isDestroyed) {
				_gate = Cast<AGate>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AGate::StaticClass(), FTransform(FRotator(),
					FVector(structureData.structureLocation, 0.0f)), ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
				if (IsValid(_gate) && _gate->StaticClass()->ImplementsInterface(UStructureable::StaticClass())) {
					_sObj = Cast<IStructureable>(_gate);
					_sObj->SetStructureData(structureData);
					UGameplayStatics::FinishSpawningActor(_gate, _gate->GetActorTransform());
				} else if(_gate)
					_gate->Destroy();
			}
		}

		//Generate Inited Ship
		for (FObjectPlacement& shipInitData : currentSectorInfo->ShipInitedData) {
			if (FMath::RandRange(0, 100) < 100 - FMath::Clamp(shipInitData.regenChanceFactor, 0, 100))
				continue;

			_ship = Cast<AShip>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AShip::StaticClass(), FTransform(FRotator(0.0f, shipInitData.rotation.Yaw, 0.0f),
				FVector(shipInitData.location, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (IsValid(_ship)) {
				_ship->InitObject(shipInitData.objectID);
				UGameplayStatics::FinishSpawningActor(_ship, _ship->GetActorTransform());
			} else if(_ship)
				_ship->Destroy();
		}
		//Generate Inited Resources
		for (FObjectPlacement& resourceInitData : currentSectorInfo->ResourceInitedData) {
			if (FMath::RandRange(0, 100) < 100 - FMath::Clamp(resourceInitData.regenChanceFactor, 0, 100))
				continue;

			_resource = Cast<AResource>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), AResource::StaticClass(), FTransform(FRotator(0.0f, resourceInitData.rotation.Yaw, 0.0f),
				FVector(resourceInitData.location, 0.0f)), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn));
			if (_resource != nullptr) {
				_resource->InitObject(resourceInitData.objectID);
				UGameplayStatics::FinishSpawningActor(_resource, _resource->GetActorTransform());
			} else if (_resource)
				_resource->Destroy();
		}
	}

	playerFactionName = loader->playerFactionName;
	factionRelationship = loader->relation;
	tempFactionRelationship = factionRelationship;
	relationwithPlayerEmpire = loader->relationwithPlayerEmpire;

	UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Relationship In save file : %d"), factionRelationship.Num());
	if (factionRelationship.Num() != factionEnumPtr->NumEnums() - 4) {
		factionRelationship.SetNumZeroed(factionEnumPtr->NumEnums() - 4);
		tempFactionRelationship.SetNumZeroed(factionEnumPtr->NumEnums() - 4);
		UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Relationship Init not Matching with save file! Some Relationship Data Will be Damaged! Set To %d."), factionRelationship.Num());
	}
	UE_LOG(LogClass, Log, TEXT("[Info][SpaceState][LoadSpaceState] Game Load Finish!"));

	OnBGMSettingAndPlay();
	return true;
}
#pragma endregion

#pragma region Get Data Or SectorData Processing
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
	isDestroying ? currentShipCapacity-- : currentShipCapacity++;
	return;
}

Peer ASpaceState::PeerIdentify(const Faction requestor, const Faction target, const bool isRealRelation) const {
	if (target == Faction::Neutral)
		return Peer::Neutral;
	if (requestor == target)
		return Peer::AllyStrong;

	Peer _result = Peer::Neutral;
	float _relation = 0.0f;

	//플레이어와 관련없는 팩션간 피아관계
	if (requestor > Faction::PlayerFoundingFaction && target > Faction::PlayerFoundingFaction) {
		switch (requestor) {
		case Faction::Empire:
			switch (target) {
			case Faction::Protectorate:				_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
			break;
		case Faction::TradeCoalition:
			switch (target) {
			case Faction::CitizenFederation:		_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
			break;
		case Faction::CitizenFederation:
			switch (target) {
			case Faction::TradeCoalition:			_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
		case Faction::Protectorate:
			switch (target) {
			case Faction::Empire:					_result = Peer::Ally;	break;
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
		case Faction::LibertyNation:
			switch (target) {
			case Faction::Pirate:					_result = Peer::EnemyStrong; break;
			default:								_result = Peer::Enemy; break;
			}
		case Faction::Pirate:
			_result = Peer::Enemy; break;
		default: break;
		}
	} 
	//플레이어와 관련있는 팩션간 피아관계
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
				_relation = FMath::Clamp(factionRelationship[FMath::Clamp((int)_targetFaction - (int)Faction::Empire, 0, factionRelationship.Num() - 1)], _define_FactionRelationshipMIN, _define_FactionRelationshipMAX);
			else
				_relation = FMath::Clamp(tempFactionRelationship[FMath::Clamp((int)_targetFaction - (int)Faction::Empire, 0, factionRelationship.Num() - 1)], _define_FactionRelationshipMIN, _define_FactionRelationshipMAX);
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

	relationshipArray = isRealRelation ? factionRelationship : tempFactionRelationship;
	return relationwithPlayerEmpire;
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
#pragma endregion

#pragma region BGM
void ASpaceState::OnBGMSettingAndPlay() {

	if (ev_BGMComponent->IsPlaying() || currentSectorInfo == nullptr)
		return;

	int _bgmIndex;

	if (currentSectorInfo->PlayerableBGM.Num() > 0) {
		_bgmIndex = FMath::RandRange(0, currentSectorInfo->PlayerableBGM.Num() - 1);

		ev_BGMCue = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *currentSectorInfo->PlayerableBGM[_bgmIndex].ToString()));
		if (ev_BGMComponent->IsValidLowLevelFast() && ev_BGMCue->IsValidLowLevelFast()) {
			ev_BGMComponent->SetSound(ev_BGMCue);
			ev_BGMComponent->Play();
		}
	}
	return;
}
#pragma endregion