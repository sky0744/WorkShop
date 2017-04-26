// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "ProjectNausForBP.h"
#include "../GameCoreSub/SafeENGINE.h"
#include "../GameCoreSub/SaveLoader.h"
#include "UserState.generated.h"




UCLASS()
class PROJECTNAUSFORBP_API AUserState : public APlayerState
{
	GENERATED_BODY()
public:
	AUserState();

#pragma region Event Calls
protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Save/Load
public:
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool NewGameSetting(Faction selectedFaction, FText userName);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool Jump(FString jumpToSector);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool TotalSave(bool isBeforeWarp = false);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool TotalLoad();

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Player")
		bool PlayerSave(USaveLoader* saver);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Player")
		bool PlayerLoad(USaveLoader* loader);
#pragma endregion

#pragma region User Data Access
public:
	UFUNCTION(BlueprintCallable,  Category = "Call Function : State")
		FText GetName();
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		Faction GetOriginFaction();

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool ShipBuy(int newShipID);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool ChangeCredit(float credit);//, FText category, FText contents);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool ChangeBounty(float _bounty);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		float GetCredit();
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		float GetBounty();

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool AddPlayerCargo(FItem addItem);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool DropPlayerCargo(FItem dropItem);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool BuyItem(FItem buyItems);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool SellItem(FItem sellItems);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool TransferItem(FItem transferItems, bool isToStationDirection);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool EquipModule(int itemSlotIndex);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool UnEquipModule(ItemType moduleType, int slotIndex);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void SetDockedStructure(TScriptInterface<IStructureable> dockingStructure);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void GetDockedStructure(TScriptInterface<IStructureable>& getStructure);
	TScriptInterface<IStructureable> DockedStructure();

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void GetUserDataItem(TArray<FItem>& setArray);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void GetUserDataSkill(TArray<FSkill>& setArray);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void AddSkillQueue(FSkill addSkill);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void DropSkillQueue(FSkill dropSkill);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void GetAchievments(TArray<int>& _achievmentsLevels);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
	bool CheckSkill(TArray<int>& skillID, TArray<int>& skillLevel);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
	float CheckCargoValue();
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
	float CheckAddItemValue(FItem item);
#pragma endregion

#pragma region Components & Variables
private:
	FText sUserName;
	int sShipID;
	Faction originFaction;
	float sCredit;
	float sBounty;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "User Data")
	TArray<FSkill> listSkill;
	UPROPERTY(BlueprintReadOnly, Category = "User Data")
	TArray<FSkill> queueSkillLearn;
	UPROPERTY(BlueprintReadOnly, Category = "User Data")
	TArray<FItem> listItem;

private:
	FString nextSectorName;
	TScriptInterface<IStructureable> dockedStructure;
	float currentCargo;
#pragma endregion

#pragma region Environment Variable
public:
	//Grahpic
		static bool ev_FullScreenMode;
		static FVector2D ev_ScreenResolution;
		static int8 ev_GrahpicLevelTextrue;
		static int8 ev_GrahpicLevelShader;
		static int8 ev_GrahpicLevelShadow;
		static int8 ev_GrahpicLevelAntiAliasing;
		static int8 ev_GrahpicLevelAnisotropicFiltering;
		static bool ev_GrahpicHDR;
		static bool ev_GrahpicSSAO;
	//Sound
		static float ev_MasterVolume;
		static float ev_BGMVolume;
		static float ev_SfxVolume;
	//Setting
		static float ev_KeyAsixSensitivity;
		static float ev_MouseAsixSensitivity;
	//Achievements

#pragma endregion
};
