// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "UserState.h"


AUserState::AUserState() {
	playerSkill = TMap<int, FSkill>();
	listItem = TArray<FItem>();

	ev_BGMClass = Cast<USoundClass>(StaticLoadObject(USoundClass::StaticClass(), NULL, TEXT("SoundClass'/Game/Resource/Sound/SoundAsset/BGM.BGM'")));
	ev_SfxClass = Cast<USoundClass>(StaticLoadObject(USoundClass::StaticClass(), NULL, TEXT("SoundClass'/Game/Resource/Sound/SoundAsset/Sfx.Sfx'")));
	ev_BGMMix = Cast<USoundMix>(StaticLoadObject(USoundMix::StaticClass(), NULL, TEXT("SoundMix'/Game/Resource/Sound/SoundAsset/BGMMixer.BGMMixer'")));
	ev_SfxMix = Cast<USoundMix>(StaticLoadObject(USoundMix::StaticClass(), NULL, TEXT("SoundMix'/Game/Resource/Sound/SoundAsset/SfxMixer.SfxMixer'")));
	FPlatformMisc::ControlScreensaver(FPlatformMisc::EScreenSaverAction::Enable);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = _define_SkillLearningTick;
}

void AUserState::BeginPlay() {
	Super::BeginPlay();

	if (!UGameplayStatics::GetCurrentLevelName(GetWorld()).Equals("MainTitle", ESearchCase::IgnoreCase)) {
		UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][BeginPlay] Current World is %s, Start Loading Save File."), *UGameplayStatics::GetCurrentLevelName(GetWorld()));
		TotalLoad();
	}
}

void AUserState::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (learningSkillId > -1 && playerSkill.Contains(learningSkillId) && playerSkill[learningSkillId].skillLevel < 5) {
		playerSkill[learningSkillId].skillLearning -= DeltaTime;
		if (playerSkill[learningSkillId].skillLearning < 0) {
			playerSkill[learningSkillId].skillLevel = FMath::Clamp(playerSkill[learningSkillId].skillLevel + 1,
				_define_SkillLevelMIN + 1, _define_SkillLevelMAX);

			USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
			FSkillData _tempSkillData;
			int _skillLearnMultiple = 0;
			if (_tempInstance) {
				_tempSkillData = _tempInstance->GetSkillData(learningSkillId);
				_skillLearnMultiple = _tempSkillData.LearningMultiple;
			}
			else _skillLearnMultiple = _define_SkillLearningMultipleUnknown;

			switch (playerSkill[learningSkillId].skillLevel) {
				case 1:		playerSkill[learningSkillId].skillLearning = _skillLearnMultiple * _define_SkillLearningTimeBase2;	break;
				case 2:		playerSkill[learningSkillId].skillLearning = _skillLearnMultiple * _define_SkillLearningTimeBase3;	break;
				case 3:		playerSkill[learningSkillId].skillLearning = _skillLearnMultiple * _define_SkillLearningTimeBase4;	break;
				case 4:		playerSkill[learningSkillId].skillLearning = _skillLearnMultiple * _define_SkillLearningTimeBase5;	break;
				case 5:		playerSkill[learningSkillId].skillLearning = -1.0f;	break;
				default:	playerSkill[learningSkillId].skillLearning = _skillLearnMultiple * _define_SkillLearningTimeBase5;	break;
			}
		}
	}
}

#pragma region Save/Load
bool AUserState::NewGameSetting(const Faction selectedFaction, const FText& userName) {

	sUserName = userName;
	int _select = 4;

	if (selectedFaction == Faction::Empire)
		_select = 0;
	else if (selectedFaction == Faction::CitizenFederation)
		_select = 1;
	else if (selectedFaction == Faction::TradeCoalition)
		_select = 2;
	else if (selectedFaction == Faction::Protectorate)
		_select = 3;

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	USaveLoader* _saver = Cast<USaveLoader>(UGameplayStatics::CreateSaveGameObject(USaveLoader::StaticClass()));
	FNewStartPlayerData _tempStartInfo = _tempInstance->GetStartProfileData(_select);

	_saver->name = sUserName;
	_saver->shipID = _tempStartInfo.StartShipID;
	_saver->credit = _tempStartInfo.StartCredit;
	_saver->sectorName = _saver->restartSector = _tempStartInfo.StartSector;
	_saver->position = _saver->restartLocation = _tempStartInfo.StartPosition;
	_saver->rotation = FRotator(0.0f, 0.0f, 0.0f);

	_saver->userSkillData = _tempStartInfo.StartSkillList;
	_saver->userLearningSkillId = -1;
	_saver->userItemData = _tempStartInfo.StartItemList;

	FShipData _tempShipData = _tempInstance->GetShipData(_tempStartInfo.StartShipID);
	_saver->shield = _tempShipData.Shield;
	_saver->armor = _tempShipData.Armor;
	_saver->hull = _tempShipData.Hull;
	_saver->power = _tempShipData.Power;

	_saver->slotTargetModule = TArray<int>();
	_saver->slotActiveModule = TArray<int>();
	_saver->slotPassiveModule = TArray<int>();
	_saver->slotSystemModule = TArray<int>();

	_saver->saveState = SaveState::NewGameCreate;
	_saver->relation = _tempStartInfo.StartFactionRelation;
	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
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
	if (!IsValid(_sectorState))
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

	USaveLoader* _loader = Cast<USaveLoader>(UGameplayStatics::LoadGameFromSlot("SaveGame", 0));
	if (!IsValid(_loader)) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Can't Find Save Files or casting SaveLoader"));
		return false;
	}

	FSectorData _tempSectorData = Cast<USafeENGINE>(GetGameInstance())->GetSectorData(_loader->sectorName);
	if (&_tempSectorData == nullptr) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Can't Access Sector Data."));
		return false;
	}

	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!IsValid(_spaceState))
		return false;

	if (_spaceState->LoadSpaceState(_loader) != true) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Space Info Load Fail."));
		return false;
	}
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalLoad] Space Info Load Finish."));

	if (PlayerLoad(_loader) == false) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][TotalLoad] Player Load Fail."));
		return false;
	}
	UE_LOG(LogClass, Log, TEXT("[Info][PlayerState][TotalLoad] Player Load Finish."));

	//Sector load -> execute with/by UserState's PlayBegin
	return true;
}
void AUserState::PlayerDeath() {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!IsValid(_tempInstance))
		return;
	FItemData _tempItemData;
	if (!IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		return;
	ASpaceState* _spaceState = Cast<ASpaceState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!IsValid(_spaceState))
		return;
	ASpaceObject* _playerPawn = Cast<ASpaceObject>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	if (!IsValid(_playerPawn) || _playerPawn->GetValue(GetStatType::currentHull) > 0.0f)
		return;

	TotalSave(true);
	USaveLoader* _Repositioning = Cast<USaveLoader>(UGameplayStatics::LoadGameFromSlot("SaveGame", 0));
	TArray<AActor*> _stationsInSector;
	TArray<AActor*> _gatesInSector;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStation::StaticClass(), _stationsInSector);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGate::StaticClass(), _gatesInSector);

	//파기 불가능한 모듈 및 아이템은 카고에 저장하는 상태에서 사망 처리
	for (int index = 0; index < _Repositioning->userItemData.Num(); index++) {
		_tempItemData = _tempInstance->GetItemData(_Repositioning->userItemData[index].itemID);
		if (!_tempItemData.isCanDrop && !_tempItemData.isCanSell && !_tempItemData.isCanReprocess)
			continue;
		_Repositioning->userItemData.RemoveAtSwap(index);
	}
	for (int& moduleID : _Repositioning->slotTargetModule) {
		_tempItemData = _tempInstance->GetItemData(moduleID);
		if (!_tempItemData.isCanDrop && !_tempItemData.isCanSell && !_tempItemData.isCanReprocess)
			USafeENGINE::AddCargo(_Repositioning->userItemData, FItem(moduleID, 1));
	}
	for (int& moduleID : _Repositioning->slotActiveModule) {
		_tempItemData = _tempInstance->GetItemData(moduleID);
		if (!_tempItemData.isCanDrop && !_tempItemData.isCanSell && !_tempItemData.isCanReprocess)
			USafeENGINE::AddCargo(_Repositioning->userItemData, FItem(moduleID, 1));
	}
	for (int& moduleID : _Repositioning->slotPassiveModule) {
		_tempItemData = _tempInstance->GetItemData(moduleID);
		if (!_tempItemData.isCanDrop && !_tempItemData.isCanSell && !_tempItemData.isCanReprocess)
			USafeENGINE::AddCargo(_Repositioning->userItemData, FItem(moduleID, 1));
	}
	for (int& moduleID : _Repositioning->slotSystemModule) {
		_tempItemData = _tempInstance->GetItemData(moduleID);
		if (!_tempItemData.isCanDrop && !_tempItemData.isCanSell && !_tempItemData.isCanReprocess)
			USafeENGINE::AddCargo(_Repositioning->userItemData, FItem(moduleID, 1));
	}
	_Repositioning->userItemData.Shrink();

	//함선 초기화 및 현상금 제거, 일정량의 크레딧 손실
	_Repositioning->shipID = 0;
	sBounty = 0.0f;
	sCredit *= FMath::FRandRange(0.2f, 0.9f);

	//섹터 내 재배치. 우선순위 : 0 - 최근에 지정한 재배치 섹터 및 위치, 1 - 랜덤한 스테이션, 2 - 랜덤한 게이트, etc - zero
	if (_spaceState->isValidSector(_Repositioning->restartSector)) {
		_Repositioning->sectorName = _Repositioning->restartSector;
		_Repositioning->position = _Repositioning->restartLocation;
	} 
	else {
		_Repositioning->sectorName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		if (_stationsInSector.Num() > 0)
			_Repositioning->position = FVector2D(_stationsInSector[FMath::RandRange(0, _stationsInSector.Num() - 1)]->GetActorLocation());
		else if (_stationsInSector.Num() > 0)
			_Repositioning->position = FVector2D(_gatesInSector[FMath::RandRange(0, _stationsInSector.Num() - 1)]->GetActorLocation());
		else _Repositioning->position = FVector2D::ZeroVector;
	}
	UGameplayStatics::SaveGameToSlot(_Repositioning, "SaveGame", 0);
	//UGameplayStatics::OpenLevel(GetWorld(), "MainTitle", TRAVEL_Absolute);
}
void AUserState::PlayerDeathProcess() {

}

bool AUserState::PlayerSave(USaveLoader* _saver) {

	if (!IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
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
	_saver->restartSector = restartSector;
	_saver->restartLocation = restartLocation;

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

	_saver->userItemData = listItem;
	for (auto& skill : playerSkill)
		_saver->userSkillData.Emplace(skill.Value);
	_saver->userLearningSkillId = learningSkillId;

	if (_saver->saveState == SaveState::UserRequest) {
		_saver->position = FVector2D(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());
		_saver->rotation = FRotator(0.0f, UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorRotation().Yaw, 0.0f);
	}
	return true;
}
bool AUserState::PlayerLoad(USaveLoader* loader) {
	if (!IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerLoad] Can't Find Player's Pawn."));
		return false;
	}
	if (!UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsA(APlayerShip::StaticClass())) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerSave] Can't Find IsA Relative about Player's Pawn."));
		return false;
	}

	APlayerShip* _obj = Cast<APlayerShip>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!IsValid(_obj)) {
		UE_LOG(LogClass, Log, TEXT("[Error][PlayerState][PlayerLoad] Fail to Get Player Unit's Infomation!"));
		return false;
	}
	sUserName = loader->name;
	sShipID = loader->shipID;
	sCredit = loader->credit;

	for (FSkill& skill : loader->userSkillData) {
		if (!playerSkill.Contains(skill.skillID))
			playerSkill.Emplace(skill.skillID, skill);
		else if (skill.skillLevel > playerSkill[skill.skillID].skillLevel && playerSkill[skill.skillID].skillLevel > 0)
			playerSkill[skill.skillID].skillLevel = FMath::Clamp(skill.skillLevel, _define_SkillLevelMIN + 1, _define_SkillLevelMAX);
	}
	learningSkillId = loader->userLearningSkillId;
	listItem = loader->userItemData;

	if(!_obj->InitObject(loader->shipID)) {
		UE_LOG(LogClass, Warning, TEXT("[Invaild Access][PlayerState][PlayerLoad] Player's Pawn Already Inited."));
		return false;
	}
	if (!_obj->LoadFromSave(loader)) {
		UE_LOG(LogClass, Warning, TEXT("[Invaild Access][PlayerState][PlayerLoad] Player's Pawn Already Inited."));
		return false;
	}
	UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->SetActorLocation(FVector(loader->position, 0.0f));
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
	if (!IsValid(_tempInstance) || !IsValid(_obj))
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
	if (!IsValid(_tempInstance))
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
	if (!IsValid(_tempInstance))
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
	if (!IsValid(_tempInstance))
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
	if (!IsValid(_tempInstance))
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
	setArray.Empty();
	for (int index = 0; index < playerSkill.Num(); index++)
		setArray.Emplace(playerSkill[index]);
}

void AUserState::GetLearningSkill(FSkill& learningSkill) const {
	if (playerSkill.Contains(learningSkillId) && learningSkillId > -1)
		learningSkill = playerSkill[learningSkillId];
	else {
		learningSkill.skillID = -1;
		learningSkill.skillLearning = -1.0f;
		learningSkill.skillLevel = -1;
	}
}

void AUserState::SetLearningSkill(const int learningId) {
	if (playerSkill.Contains(learningId)) {
		learningSkillId = learningId;
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIUser();
	}
}

void AUserState::StopLearningSkill() {
	SetLearningSkill(-1);
}

bool AUserState::NewLearningSkill(const int NewLearningSkillId) {
	//기본적인 체크
	if (playerSkill.Contains(NewLearningSkillId) || NewLearningSkillId < 0)
		return false;
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FSkillData _tempSkillData;
	int _skillLearnMultiple = 0;

	if (_tempInstance) {
		_tempSkillData = _tempInstance->GetSkillData(NewLearningSkillId);
		_skillLearnMultiple = _tempSkillData.LearningMultiple;
	} else
		return false;

	//DataTable로부터 획득한 데이터를 체크
	if (playerSkill.Contains(_tempSkillData.SkillID))
		return false;
	if (_tempSkillData.RequireSkillBookID > -1 && USafeENGINE::FindItemSlot(listItem, FItem(_tempSkillData.RequireSkillBookID, 1)) < 0)
		return false;
	if (!CheckSkill(_tempSkillData.RequireSkill))
		return false;

	playerSkill.Emplace(_tempSkillData.SkillID, FSkill(_tempSkillData.SkillID, 0, _skillLearnMultiple * _define_SkillLearningTimeBase1));
	learningSkillId = _tempSkillData.SkillID;
	return true;
}

void AUserState::GetAchievments(TArray<int>& _achievmentsLevels) const {

}

bool AUserState::CheckSkill(const TArray<FSkill>& checkSkill) const {

	for (int index = 0; index < checkSkill.Num(); index++) {
		if (!playerSkill.Contains(checkSkill[index].skillID))
			return false;
		if (playerSkill[checkSkill[index].skillID].skillLevel < checkSkill[index].skillLevel)
			return false;
	}
	return true;
}

float AUserState::CheckCargoValue() const {
	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	if (!IsValid(_tempInstance))
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
	if (!IsValid(_tempInstance))
		return -1.0f;

	FItemData _tempItemData = _tempInstance->GetItemData(item.itemID);
	return _tempItemData.CargoVolume * item.itemAmount;
}

bool AUserState::SetRestartLocation() {
	
	APawn* _tempPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(_tempPawn))
		return false;

	restartSector = UGameplayStatics::GetCurrentLevelName(GetWorld());
	restartLocation = FVector2D(_tempPawn->GetActorLocation());
	return true;
}
#pragma endregion

#pragma region Debugging Cheat
void AUserState::CheatCommand(CheatType cheatType, UPARAM(ref) FString& parameters) {
	ASpaceHUDBase* _hud = Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	FString _parameter1;
	FString _parameter2;
	bool _isSplited = parameters.Split(" ", &_parameter1, &_parameter2);
	bool _isCheckSuccess = false;

	switch (cheatType) {
	case CheatType::AddCredit:
		if (!_isSplited) {
			ChangeCredit(FMath::Abs(FCString::Atoi(*parameters)));
			_isCheckSuccess = true;
		}
		break;
	case CheatType::RemoveCredit:
		if (!_isSplited) 
			_isCheckSuccess = ChangeCredit(-FMath::Abs(FCString::Atoi(*parameters)));
		break;
	case CheatType::AddItem:
		if (_isSplited) 
			_isCheckSuccess = AddPlayerCargo(FItem(FCString::Atoi(*_parameter1), FCString::Atoi(*_parameter2)));
		break;
	case CheatType::RemoveItem:
		if (_isSplited) 
			_isCheckSuccess = DropPlayerCargo(FItem(FCString::Atoi(*_parameter1), FCString::Atoi(*_parameter2)));
		break;
	case CheatType::AddSkill:
		if (_isSplited) {
			playerSkill.Emplace(FCString::Atoi(*_parameter1), FSkill(FCString::Atoi(*_parameter1), FCString::Atoi(*_parameter2)));
			_isCheckSuccess = true;
		}
		break;
	case CheatType::RemoveSkill:
		if (!_isSplited) {
			playerSkill.Remove(FCString::Atoi(*parameters));
			_isCheckSuccess = true;
		}
		break;
	case CheatType::GetSkillList:
		if (!_hud)
			break;
		for (auto& skills : playerSkill)
			_hud->AddUILogMessage(FText::Format(NSLOCTEXT("UIText", "LogMessage_DebugCommand_GetSkillList", "스킬 ID : {ID}, 레벨 : {Level}"), skills.Value.skillID, skills.Value.skillLevel) , FColor::Blue);
		_isCheckSuccess = true;
		break;
	case CheatType::WarpTo:
		if (!_isSplited)
			_isCheckSuccess = Jump(parameters);
		break;
	case CheatType::BGMVolume:
		if (!_isSplited && ev_BGMMix && ev_BGMClass) {
				UGameplayStatics::SetSoundMixClassOverride(GetWorld(), ev_BGMMix, ev_BGMClass, FMath::Clamp(FCString::Atof(*parameters), 0.0f, 1.0f));
				_isCheckSuccess = true;
			}
		break;
	case CheatType::SfxVolume:
		if (!_isSplited && ev_SfxMix && ev_SfxClass) {
			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), ev_SfxMix, ev_SfxClass, FMath::Clamp(FCString::Atof(*parameters), 0.0f, 1.0f));
			_isCheckSuccess = true;
		}
		break;
	default:
		break;
	}
	if (_hud) {
		if (_isCheckSuccess)
			_hud->AddUILogMessage(NSLOCTEXT("UIText", "LogMessage_DebugCommandResult_Success", "커맨드 적용 성공"), FColor::White);
		else
			_hud->AddUILogMessage(NSLOCTEXT("UIText", "LogMessage_DebugCommandResult_Fail", "커맨드 적용 실패"), FColor::Red);
	}
	return;
}
#pragma endregion

#pragma region Changing Environment Setting Functions
/*
float AUserState::Ev_ChangeBGMVolume(float volume) {

	if (!ev_BGMClass->IsValidLowLevelFast() || !ev_BGMMix->IsValidLowLevelFast())
		ev_BGMVolume;

	ev_BGMVolume = FMath::Clamp(volume, _define_ev_SoundVolumeMIN, _define_ev_SoundVolumeMAX);
	UGameplayStatics::SetSoundMixClassOverride(GEngine->GetWorld(), ev_BGMMix, ev_BGMClass, ev_BGMVolume, 1.0f, 1.0f, true);

	return ev_BGMVolume;
}

float AUserState::Ev_ChangeSfxVolume(float volume) {

	if (!ev_BGMClass->IsValidLowLevelFast() || !ev_BGMMix->IsValidLowLevelFast())
		return ev_SfxVolume;

	ev_SfxVolume = FMath::Clamp(volume, _define_ev_SoundVolumeMIN, _define_ev_SoundVolumeMAX);
	UGameplayStatics::SetSoundMixClassOverride(GEngine->GetWorld(), ev_SfxMix, ev_SfxClass, ev_SfxVolume, 1.0f, 1.0f, true);

	return ev_SfxVolume;
}
*/
#pragma endregion
