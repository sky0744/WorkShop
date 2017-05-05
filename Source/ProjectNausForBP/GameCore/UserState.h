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

#pragma region Save/Load & Player Death
public:
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool NewGameSetting(const Faction selectedFaction, const FText& userName);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool Jump(const FString& jumpToSector);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool TotalSave(const bool isBeforeWarp = false);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool TotalLoad();
	void PlayerDeath();

private:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Player")
		bool PlayerSave(USaveLoader* saver);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Player")
		bool PlayerLoad(USaveLoader* loader);
#pragma endregion

#pragma region User Data Access
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		const FText& GetName() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		Faction GetOriginFaction() const;

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool ShipBuy(const int newShipID);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/**	유저에게 크레딧을 부여합니다.
		*	@param varianceCredit - 부여 크레딧. 범위는 -9999999999999999.0f ~ 9999999999999999.0f
		*	@return 변동 성공 여부(유효성)
		*/
		bool ChangeCredit(float varianceCredit);//, FText category, FText contents);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/**	유저가 보유한 크레딧의 양을 확인합니다.
		*	@return 보유 크레딧
		*/
		float GetCredit() const;
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/**	유저에게 현상금을 부여 또는 탕감합니다.
		*	@param varianceBounty - 부여 현상금.
		*	@return 변동 성공 여부(유효성)
		*/
		bool ChangeBounty(float varianceBounty);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/**	유저에게 부여된 현상금을 확인합니다.
		*	@return 부여 현상금
		*/
		float GetBounty() const;
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/**	유저에게 명성을 부여합니다.
		*	@param varianceRenown - 명성. 범위는 -1000.0f ~ 1000.0f
		*/
		void ChangeRenown(const Peer peer, float varianceRenown);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/**	유저의 명성을 확인합니다.
		*	@return 명성 수치
		*/
		float GetRenown() const;

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool AddPlayerCargo(FItem addItem);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool DropPlayerCargo(FItem dropItem);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool BuyItem(FItem buyItems);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool SellItem(FItem sellItems);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool TransferItem(const FItem transferItems, const bool isToStationDirection);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool EquipModule(const int itemSlotIndex);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool UnEquipModule(const ItemType moduleType, const int slotIndex);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void SetDockedStructure(const TScriptInterface<IStructureable> dockingStructure);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		void GetDockedStructure(TScriptInterface<IStructureable>& getStructure) const;
	TScriptInterface<IStructureable> DockedStructure() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		void GetUserDataItem(TArray<FItem>& setArray) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		void GetUserDataSkill(TArray<FSkill>& setArray) const;
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void AddSkillQueue(const FSkill addSkill);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void DropSkillQueue(const FSkill dropSkill);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		void GetAchievments(TArray<int>& _achievmentsLevels) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		bool CheckSkill(const TArray<FSkill>& checkSkill) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		float CheckCargoValue() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		float CheckAddItemValue(const FItem item) const;

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool SetRestartLocation();
#pragma endregion

#pragma region Components & Variables
private:
	FText sUserName;
	int sShipID;
	Faction originFaction;
	float sCredit;
	float sRenown;
	float sBounty;

	FString restartSector;
	FVector restartLocation;
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
