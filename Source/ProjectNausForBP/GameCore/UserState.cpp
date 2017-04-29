// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "UserState.h"


AUserState::AUserState() {

}

void AUserState::BeginPlay() {
	Super::BeginPlay();

	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != "MainTitle") {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][BeginPlay] Current World is %s, Start Loading Save File."), *UGameplayStatics::GetCurrentLevelName(GetWorld()));
		TotalLoad();
	}
	else 
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][BeginPlay] Current World is MainTitle, so Not Loaded Save File."));
}

#pragma region Save/Load
bool AUserState::NewGameSetting(const Faction selectedFaction, const FText& userName) {

	sUserName = userName;
	int _select = 4;

	if (selectedFaction == Faction::PrimusEmpire)
		_select = 0;
	else if (selectedFaction == Faction::FreeCitizenFederation)
		_select = 1;
	else if (selectedFaction == Faction::FlorenceTradeCoalition)
		_select = 2;
	else if (selectedFaction == Faction::ValenciaProtectorate)
		_select = 3;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	USaveLoader* _saver = Cast<USaveLoader>(UGameplayStatics::CreateSaveGameObject(USaveLoader::StaticClass()));
	FNewStartPlayerData _tempStartInfo = _tempInstance->GetStartProfileData(_select);

	_saver->name = sUserName;
	_saver->shipID = _tempStartInfo.StartShipID;
	_saver->credit = _tempStartInfo.StartCredit;
	_saver->sectorName = _tempStartInfo.StartSector;
	_saver->position = _tempStartInfo.StartPosition;
	_saver->rotation = FRotator(0.0f, 0.0f, 0.0f);

	_saver->skillList = _tempStartInfo.StartSkillList;
	_saver->itemList = _tempStartInfo.StartItemList;

	FShipData _tempShipData = _tempInstance->GetShipData(_tempStartInfo.StartShipID);
	_saver->shield = _tempShipData.Shield;// *(1.0f + tempShipData.BonusShield);
	_saver->armor = _tempShipData.Armor;// *(1.0f + tempShipData.BonusArmor);
	_saver->hull = _tempShipData.Hull;// *(1.0f + tempShipData.BonusHull);
	_saver->power = _tempShipData.Power;// *(1.0f + tempShipData.BonusPower);

	_saver->slotTargetModule = TArray<int>();
	_saver->slotActiveModule = TArray<int>();
	_saver->slotPassiveModule = TArray<int>();
	_saver->slotSystemModule = TArray<int>();

	_saver->saveState = SaveState::NewGameCreate;
	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	_saver->relation = _tempStartInfo.StartFactionRelation;

	if (_spaceState->SaveSpaceState(_saver) != true) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][NewGameSetting] Space Info Save Fail."));
		return false;
	}
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][NewGameSetting] Space Info Save Finish."));

	if (UGameplayStatics::SaveGameToSlot(_saver, "SaveGame", 0) == false) {
		UE_LOG(LogClass, Warning, TEXT("[Warning][PlayerState][TotalSave] SaveLoader Can't Save in SaveSlot. Try again."));
		return false;
	}

	FSectorData _tempSectorData = _tempInstance->GetSectorData(_tempStartInfo.StartSector);
	UGameplayStatics::OpenLevel(GetWorld(), _tempSectorData.nSectorName, TRAVEL_Absolute);
	return true;
}
bool AUserState::Jump(const FString& jumpToSector) {

	ASpaceState* _sectorState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!USafeENGINE::IsValid(_sectorState))
		return false;
	if (!_sectorState->isValidSector(jumpToSector))
		return false;

	nextSectorName = jumpToSector;
	if (AUserState::TotalSave(true) == false)
		return false;

	UGameplayStatics::OpenLevel(GetWorld(), *nextSectorName, TRAVEL_Absolute);
	return true;
}

bool AUserState::TotalSave(const bool isBeforeWarp) {
	USaveLoader* _saver = Cast<USaveLoader>(UGameplayStatics::CreateSaveGameObject(USaveLoader::StaticClass()));

	if (isBeforeWarp == true) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalSave] Save to Warp Another Sector."));
		_saver->sectorName = nextSectorName;
		_saver->saveState = SaveState::BeforeWarp;
	}
	else {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalSave] Save by Player Request."));
		_saver->sectorName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		_saver->saveState = SaveState::UserRequest;
	}
	
	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));

	if (_spaceState->SaveSpaceState(_saver) != true) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalSave] Space Info Save Fail."));
		return false;
	}
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalSave] Space Info Save Finish."));

	if (PlayerSave(_saver) != true) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalSave] Player Info Save Fail."));
		return false;
	}
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalSave] Player Info Save Finish."));

	if (UGameplayStatics::SaveGameToSlot(_saver, "SaveGame", 0) == false) {
		UE_LOG(LogClass, Warning, TEXT("[Warning][PlayerState][TotalSave] SaveLoader Can't Save in SaveSlot. Try again."));
		return false;
	}
	else return true;
}
bool AUserState::TotalLoad() {

	USaveLoader* loader = Cast<USaveLoader>(UGameplayStatics::LoadGameFromSlot("SaveGame", 0));
	if (!USafeENGINE::IsValid(loader)) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Can't Find Save Files or casting SaveLoader"));
		return false;
	}

	FSectorData _tempSectorData = Cast<USafeENGINE>(GetGameInstance())->GetSectorData(loader->sectorName);
	if (&_tempSectorData == nullptr) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Can't Access Sector Data."));
		return false;
	}

	ASpaceState* spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!USafeENGINE::IsValid(spaceState))
		return false;

	if (spaceState->LoadSpaceState(loader) != true) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Space Info Load Fail."));
		return false;
	}
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalLoad] Space Info Load Finish."));

	if (PlayerLoad(loader) == false) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Player Load Fail."));
		return false;
	}
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalLoad] Player Load Finish."));

	//Sector load -> execute with/by UserState's PlayBegin
	return true;
}
void AUserState::PlayerDeath() {
	if (!USafeENGINE::IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		return;
	ASpaceObject* _playerPawn = Cast<ASpaceObject>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	if (!USafeENGINE::IsValid(_playerPawn) || _playerPawn->GetValue(GetStatType::currentHull) > 0.0f)
		return;

	
}

bool AUserState::PlayerSave(USaveLoader* _saver) {

	if (!USafeENGINE::IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerSave] Can't Find Player's Pawn."));
		return false;
	}
	if (!UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsA(APlayerShip::StaticClass())) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerSave] Can't Find IsA Relative about Player's Pawn."));
		return false;
	}

	_saver->name = sUserName;
	_saver->shipID = sShipID;
	_saver->credit = sCredit;

	APlayerShip* _obj = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	_saver->shield = _obj->GetValue(GetStatType::currentShield);
	_saver->armor = _obj->GetValue(GetStatType::currentArmor);
	_saver->hull = _obj->GetValue(GetStatType::currentHull);
	_saver->power = _obj->GetValue(GetStatType::currentPower);

	_obj->GetModule(ItemType::TargetModule, _saver->slotTargetModule);
	_obj->GetModule(ItemType::ActiveModule, _saver->slotActiveModule);
	_obj->GetModule(ItemType::PassiveModule, _saver->slotPassiveModule);
	_obj->GetModule(ItemType::SystemModule, _saver->slotSystemModule);
	_obj->GetTargetModuleAmmo(_saver->targetModuleAmmo);

	_saver->itemList = listItem;
	_saver->skillList = listSkill;

	if (_saver->saveState == SaveState::UserRequest) {
		_saver->position = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
		_saver->rotation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorRotation();
		_saver->rotation.Pitch = _saver->rotation.Roll = 0.0f;
	}
	return true;
}
bool AUserState::PlayerLoad(USaveLoader* loader) {
	if (!USafeENGINE::IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerLoad] Can't Find Player's Pawn."));
		return false;
	}
	if (!UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsA(APlayerShip::StaticClass())) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerSave] Can't Find IsA Relative about Player's Pawn."));
		return false;
	}

	APlayerShip* _obj = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!USafeENGINE::IsValid(_obj)) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerLoad] Fail to Get Player Unit's Infomation!"));
		return false;
	}
	sUserName = loader->name;
	sShipID = loader->shipID;
	sCredit = loader->credit;

	listItem = loader->itemList;
	listSkill = loader->skillList;

	if(!_obj->InitObject(loader->shipID)) {
		UE_LOG(LogClass, Warning, TEXT("[Invaild Access][PlayerState][PlayerLoad] Player's Pawn Already Inited."));
		return false;
	}
	if (!_obj->LoadFromSave(loader)) {
		UE_LOG(LogClass, Warning, TEXT("[Invaild Access][PlayerState][PlayerLoad] Player's Pawn Already Inited."));
		return false;
	}

	UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->SetActorLocation(FVector(loader->position.X, loader->position.Y, 0.0f));
	UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->SetActorRotation(FRotator(0.0f, loader->rotation.Yaw, 0.0f));
	return true;
}
#pragma endregion

#pragma region User Data Access
const FText& AUserState::GetName() const {
	return sUserName;
}

Faction AUserState::GetOriginFaction() const {
	return originFaction;
}

bool AUserState::ShipBuy(const int newShipID) {

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	APlayerShip* _obj = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!USafeENGINE::IsValid(_tempInstance) || !USafeENGINE::IsValid(_obj))
		return false;

	FShipData _tempShipData = _tempInstance->GetShipData(newShipID);
	TArray<int> _moduleToBeRemoved;
	bool _isModuleEmptyCheck = true;

	if (_tempShipData.ShipValue > sCredit) 	{
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][ShipBuy] The credit you have is lower than the price of the ship. Can't buying this ship."));
		return false;
	}

	if (currentCargo > _tempShipData.Cargo) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][ShipBuy] The current cargo volume is larger than the cargo of the ship to be purchased. Can't buying this ship."));
		return false;
	}

	if (CheckSkill(_tempShipData.RequireSkills) == false) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][ShipBuy] There is not enough skill to operate the ship. Can't buying this ship."));
		return false;
	}

	_obj->GetModule(ItemType::TargetModule, _moduleToBeRemoved);
	for (int index = 0; index < _moduleToBeRemoved.Num(); index++)
		if (_moduleToBeRemoved[index] != 0) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ShipBuy] Target Module Not Empty, please Check Module Equip State."));
			return false;
		}
	_obj->GetModule(ItemType::ActiveModule, _moduleToBeRemoved);
	for (int index = 0; index < _moduleToBeRemoved.Num(); index++)
		if (_moduleToBeRemoved[index] != 0) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ShipBuy] Active Module Not Empty, please Check Module Equip State."));
			return false;
		}
	_obj->GetModule(ItemType::PassiveModule, _moduleToBeRemoved);
	for (int index = 0; index < _moduleToBeRemoved.Num(); index++)
		if (_moduleToBeRemoved[index] != 0) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ShipBuy] Passive Module Not Empty, please Check Module Equip State."));
			return false;
		}
	_obj->GetModule(ItemType::SystemModule, _moduleToBeRemoved);
	for (int index = 0; index < _moduleToBeRemoved.Num(); index++)
		if (_moduleToBeRemoved[index] != 0) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ShipBuy] System Module Not Empty, please Check Module Equip State."));
			return false;
		}

	ChangeCredit(-_tempShipData.ShipValue);
	_tempShipData = _tempInstance->GetShipData(_obj->GetObjectID());
	ChangeCredit(_tempShipData.ShipValue * 0.3f);
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ShipBuy] Previously operated ships are sold to shipyards."));

	if (!_obj->InitObject(newShipID))
		return false;
	
	sShipID = newShipID;
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ShipBuy] The ship purchase was successful."));
	return true;
}

bool AUserState::ChangeCredit(float varianceCredit) {//, FText category, FText contents) {
	if (sCredit < 0.0f && varianceCredit < 0.0f)
		return false;

	sCredit = FMath::Clamp(sCredit + varianceCredit, _define_CreditMIN, _define_CreditMAX);
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ChangeCredit] Credit %s : %.0f."), varianceCredit >= 0.0f ? TEXT("Get") : TEXT("Lost"), varianceCredit);
	return true;
}

float AUserState::GetCredit() const {

	return sCredit;
}

bool AUserState::ChangeBounty(float varianceBounty) {//, FText category, FText contents) {
	if (sBounty + varianceBounty > _define_CreditMAX && varianceBounty > 0.0f)
		varianceBounty = 0.0f;

	sBounty = FMath::Clamp(sBounty + varianceBounty, 0.0f, _define_CreditMAX);
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ChangeBounty] Bounty %s : %.0f."), varianceBounty >= 0.0f ? TEXT("Add") : TEXT("Cancellation"), varianceBounty);
	return true;
}

float AUserState::GetBounty() const {

	return sBounty;
}

void AUserState::ChangeRenown(const Peer peer, float varianceRenown) {

	if (peer > Peer::Enemy)
		varianceRenown *= _define_SPToRenownNotHostile;
	else
		varianceRenown *= _define_SPToRenownHostile;

	sRenown = FMath::Clamp(sRenown + varianceRenown, _define_RenownMIN, _define_RenownMAX);
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][ChangeBounty] Renown %s : %.0f."), varianceRenown > 0.0f ? TEXT("Add") : TEXT("Cancellation"), varianceRenown);
}

float AUserState::GetRenown() const {

	return sRenown;
}

bool AUserState::AddPlayerCargo(FItem addItem) {
	if (!UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsA(APlayerShip::StaticClass()))
		return false;
	if (addItem.itemID < 0 || addItem.itemAmount < 1)
		return true;

	ASpaceObject* _obj = Cast<ASpaceObject>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	currentCargo = CheckCargoValue();
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][AddPlayerCargo] Weight : %f, Max Weight : %f"), currentCargo + CheckAddItemValue(addItem), _obj->GetValue(GetStatType::maxCargo));
	if (currentCargo + CheckAddItemValue(addItem) <= _obj->GetValue(GetStatType::maxCargo)) {
		
		if (USafeENGINE::AddCargo(listItem, addItem)) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][AddPlayerCargo] Item Adding Finish"));
		}
		else UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][AddPlayerCargo] Can't Adding Item!"));
		
		for(int index = 0; index < listItem.Num(); index++)
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][AddPlayerCargo] Item %d, Amount : %d"), listItem[index].itemID, listItem[index].itemAmount);
		
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUICargo();
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
		return true;
	}
	else if (currentCargo + CheckAddItemValue(addItem) > _obj->GetValue(GetStatType::maxCargo)) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][AddPlayerCargo] Cargo Overwight! Adding Item Canceled"));
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUICargo();
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
		return false;
	}
	else{
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][AddPlayerCargo] Cargo Calculate Error! Adding Item Canceled"));
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUICargo();
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
		return false;
	}
}

bool AUserState::DropPlayerCargo(FItem dropItem) {
	if (USafeENGINE::DropCargo(listItem, dropItem)) {
		currentCargo = CheckCargoValue();
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][DropPlayerCargo] Weight : %f"), currentCargo);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUICargo();
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
		return true;
	}
	
	else {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][DropPlayerCargo] Cargo Drop Error! Drop Canceled"));
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUICargo();
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
		return false;
	}
}

bool AUserState::BuyItem(FItem buyItems) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!USafeENGINE::IsValid(_tempInstance))
		return false;

	FStructureInfo* _structureInfo;
	int _findSlotInSeller;
	int _findSlotInBuyer;
	int _lowerAmount;
	int _upperAmount;
	float _totalPaymentCredit;

	if (dockedStructure.GetObjectRef()->GetClass() == AStation::StaticClass())
		_structureInfo = Cast<AStation>(dockedStructure.GetObjectRef())->GetStructureDataPointer();
	else if (dockedStructure.GetObjectRef()->GetClass() == AGate::StaticClass())
		_structureInfo = Cast<AGate>(dockedStructure.GetObjectRef())->GetStructureDataPointer();
	else return false;

	_findSlotInSeller = USafeENGINE::FindItemSlot(_structureInfo->itemList, buyItems);
	_findSlotInBuyer = USafeENGINE::FindItemSlot(_structureInfo->playerItemList, buyItems);
	if (_findSlotInSeller < 0)
		return false;

	buyItems.itemAmount = FMath::Max(0, buyItems.itemAmount);

	_upperAmount = _structureInfo->itemList[_findSlotInSeller].itemAmount;
	_lowerAmount = _structureInfo->itemList[_findSlotInSeller].itemAmount - buyItems.itemAmount;

	_totalPaymentCredit = _tempInstance->CalculateCreditForTrade(buyItems.itemID, _lowerAmount, _upperAmount, true);
	if (ChangeCredit(_totalPaymentCredit)) {
		if (USafeENGINE::AddCargo(_structureInfo->playerItemList, buyItems)) {
			if (USafeENGINE::DropCargo(_structureInfo->itemList, buyItems)) {
				Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
				return true;
			}
			else {
				USafeENGINE::DropCargo(_structureInfo->playerItemList, buyItems);
				ChangeCredit(_totalPaymentCredit);
				Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
				return false;
			}
		}
		else {
			ChangeCredit(_totalPaymentCredit);
			Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
			return false;
		}
	}
	return false;
}

bool AUserState::SellItem(FItem sellItems) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!USafeENGINE::IsValid(_tempInstance))
		return false;

	FStructureInfo* _structureInfo;
	int _findSlotInSeller;
	int _findSlotInBuyer;
	int _lowerAmount;
	int _upperAmount;
	float totalPaymentCredit;

	if (dockedStructure.GetObjectRef()->GetClass() == AStation::StaticClass())
		_structureInfo = Cast<AStation>(dockedStructure.GetObjectRef())->GetStructureDataPointer();
	else if (dockedStructure.GetObjectRef()->GetClass() == AGate::StaticClass())
		_structureInfo = Cast<AGate>(dockedStructure.GetObjectRef())->GetStructureDataPointer();
	else return false;

	_findSlotInSeller = USafeENGINE::FindItemSlot(_structureInfo->playerItemList, sellItems);
	_findSlotInBuyer = USafeENGINE::FindItemSlot(_structureInfo->itemList, sellItems);
	if (_findSlotInSeller < 0)
		return false;

	sellItems.itemAmount = FMath::Max(0, sellItems.itemAmount);
	if (_findSlotInBuyer < 0) {
		_lowerAmount = 0;
		_upperAmount = sellItems.itemAmount;
	} else {
		_upperAmount = _structureInfo->itemList[_findSlotInBuyer].itemAmount + sellItems.itemAmount;
		_lowerAmount = _structureInfo->itemList[_findSlotInBuyer].itemAmount;
	}
	totalPaymentCredit = _tempInstance->CalculateCreditForTrade(sellItems.itemID, _lowerAmount, _upperAmount, true);
	if (ChangeCredit(totalPaymentCredit)) {
		if (USafeENGINE::AddCargo(_structureInfo->itemList, sellItems)) {
			if (USafeENGINE::DropCargo(_structureInfo->playerItemList, sellItems)) {
				Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
				return true;
			}
			USafeENGINE::DropCargo(_structureInfo->itemList, sellItems);
			ChangeCredit(-totalPaymentCredit);
			Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
			return false;
		}
	} else {
		ChangeCredit(-totalPaymentCredit);
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
		return false;
	}
	return false;
}

bool AUserState::TransferItem(const FItem transferItems, const bool isToStationDirection) {
	int _findSlot = -1;

	FStructureInfo* _structureInfo;
	if (dockedStructure.GetObjectRef()->GetClass() == AStation::StaticClass())
		_structureInfo = Cast<AStation>(dockedStructure.GetObjectRef())->GetStructureDataPointer();
	else if (dockedStructure.GetObjectRef()->GetClass() == AGate::StaticClass())
		_structureInfo = Cast<AGate>(dockedStructure.GetObjectRef())->GetStructureDataPointer();
	else return false;

	if (isToStationDirection)
		_findSlot = USafeENGINE::FindItemSlot(listItem, transferItems);
	else
		_findSlot = USafeENGINE::FindItemSlot(_structureInfo->playerItemList, transferItems);

	if (_findSlot < 0)
		return false;

	if (isToStationDirection) {
		if (USafeENGINE::AddCargo(_structureInfo->playerItemList, transferItems)) {
			if (DropPlayerCargo(transferItems)) {
				Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
				return true;
			}
			else
				USafeENGINE::DropCargo(_structureInfo->playerItemList, transferItems);
		}
	}
	else {
		if (AddPlayerCargo(transferItems)) {
			if (USafeENGINE::DropCargo(_structureInfo->playerItemList, transferItems)) {
				Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
				return true;
			}
			else
				DropPlayerCargo(transferItems);
		}
	}
	Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
	return false;
}

bool AUserState::EquipModule(const int itemSlotIndex) {
	
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!USafeENGINE::IsValid(_tempInstance))
		return false;
	if (!UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsA(APlayerShip::StaticClass()))
		return false;

	APlayerShip* _obj = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FShipData _tempShipData = _tempInstance->GetShipData(_obj->GetObjectID());
	FItemData _tempItemData = _tempInstance->GetItemData(listItem[itemSlotIndex].itemID);

	switch (_tempItemData.Type) {
	case ItemType::TargetModule:
	case ItemType::ActiveModule:
	case ItemType::PassiveModule:
	case ItemType::SystemModule:
		break;
	default:
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] This item is not Module."));
		return false;
	}
	if (CheckSkill(_tempItemData.RequireSkill) == false) {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] because of the player's skill state is lower than the skill requirement, It is impossible to wear the module."));
		return false;
	}

	switch (_tempItemData.ModuleSize) {
	case ModuleSize::Capital:
		switch (_tempShipData.Shipclass) {
		case ShipClass::Carrier:
		case ShipClass::BattleShip:
			break;
		default:
			UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] Module Size Not Matched Ship Class."));
			return false;
		}
	case ModuleSize::XLarge:
		switch (_tempShipData.Shipclass) {
		case ShipClass::Carrier:
		case ShipClass::BattleShip:
		case ShipClass::BattleCruiser:
		case ShipClass::Hulk:
			break;
		default:
			UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] Module Size Not Matched Ship Class."));
			return false;
		}
		break;
	case ModuleSize::Large:
		switch (_tempShipData.Shipclass) {
		case ShipClass::BattleCruiser:
		case ShipClass::Cruiser:
		case ShipClass::Hulk:
		case ShipClass::MiningBarge:
		case ShipClass::Freighter:
			break;
		default:
			UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] Module Size Not Matched Ship Class."));
			return false;
		}
		break;
	case ModuleSize::Medium:
		switch (_tempShipData.Shipclass) {
		case ShipClass::Destroyer:
		case ShipClass::TransfortShip:
		case ShipClass::MiningBarge:
			break;
		default:
			UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] Module Size Not Matched Ship Class."));
			return false;
		}
		break;
	case ModuleSize::Small:
		switch (_tempShipData.Shipclass) {
		case ShipClass::Frigate:
		case ShipClass::MiningShip:
			break;
		default:
			UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] Module Size Not Matched Ship Class."));
			return false;
		}
		break;
	default:
		return false;
	}

	if (_obj->EquipModule(_tempItemData.ItemID) == true) {
		if (USafeENGINE::DropCargo(listItem, FItem(_tempItemData.ItemID, 1))) {
			Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationOnRequest();
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][EquipModule] Equip Finish."));
			return true;
		}
		else {
			UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] Item is invaild. Equip Fail."));
			return false;
		}
	}
	else {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][EquipModule] Equip Fail."));
		return false;
	}
}

bool AUserState::UnEquipModule(const ItemType moduleType, const int slotIndex) {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!USafeENGINE::IsValid(_tempInstance))
		return false;
	if (!UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsA(APlayerShip::StaticClass()))
		return false;

	APlayerShip* _obj = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FItemData _tempModuleData;
	TArray<int> _tempModuleList;
	TArray<float> _tempfloatArray;

	switch (moduleType) {
	case ItemType::TargetModule:
	case ItemType::ActiveModule:
	case ItemType::PassiveModule:
	case ItemType::SystemModule:
		_obj->GetModule(moduleType, _tempModuleList);
		_tempModuleData = _tempInstance->GetItemData(_tempModuleList[slotIndex]);
		break;
	default:
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][UnEquipModule] This is not Module. UnEquip Fail."));
		return false;
	}
	FItem _unEquipedItem = FItem(_tempModuleData.ItemID, 1);
	if (AddPlayerCargo(_unEquipedItem) == true) {
		if (_obj->UnEquipModule(moduleType, slotIndex) == true) {
			UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][UnEquipModule] UnEquip Finish."));
			return true;
		}
		else {
			AddPlayerCargo(_unEquipedItem);
			UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][UnEquipModule] UnEquip Fail. Add Item Cancel."));
			return false;
		}
	}
	else {
		UE_LOG(LogClass, Log, TEXT("[Warning][PlayerState][UnEquipModule] Overweight, UnEquip Fail."));
		return false;
	}
}

void AUserState::SetDockedStructure(const TScriptInterface<IStructureable> dockingStructure) {
	dockedStructure = dockingStructure;
}

void AUserState::GetDockedStructure(TScriptInterface<IStructureable>& getStructure) const {
	getStructure = dockedStructure;
}

TScriptInterface<IStructureable> AUserState::DockedStructure() const {
	return dockedStructure;
}

void AUserState::GetUserDataItem(TArray<FItem>& setArray) const {
	setArray = listItem;
}

void AUserState::GetUserDataSkill(TArray<FSkill>& setArray) const {
	setArray = listSkill;
}

void AUserState::AddSkillQueue(const FSkill addSkill){
	
}

void AUserState::DropSkillQueue(const FSkill dropSkill) {
	
}

void AUserState::GetAchievments(TArray<int>& _achievmentsLevels) const {

}

bool AUserState::CheckSkill(const TArray<FSkill>& checkSkill) const {
	bool _isFind;
	for (int index1 = 0; index1 < checkSkill.Num(); index1++) {
		_isFind = false;
		for (int index2 = 0; index2 < listSkill.Num(); index2++) {
			if (listSkill[index2].skillID == checkSkill[index1].skillID) {
				if (listSkill[index2].skillLevel >= checkSkill[index1].skillLevel)
					_isFind = true;
				break;
			}
		}
		if (_isFind == true) 
			continue;
		else return false;
	}
	return true;
}

float AUserState::CheckCargoValue() const {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!USafeENGINE::IsValid(_tempInstance))
		return -1.0f;

	FItemData _tempItemData;
	float _value = 0;

	for (int index = 0; index < listItem.Num(); index++) {
		_tempItemData = _tempInstance->GetItemData(listItem[index].itemID);
		_value += _tempItemData.CargoVolume * listItem[index].itemAmount;
	}
	return _value;
}

float AUserState::CheckAddItemValue(const FItem item) const {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!USafeENGINE::IsValid(_tempInstance))
		return -1.0f;

	FItemData _tempItemData = _tempInstance->GetItemData(item.itemID);
	return _tempItemData.CargoVolume * item.itemAmount;
}
#pragma endregion