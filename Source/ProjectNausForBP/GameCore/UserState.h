// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "ProjectNausForBP.h"
#include "../GameCoreSub/SafeENGINE.h"
#include "../GameCoreSub/SaveLoader.h"
#include "UserState.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API AUserState : public APlayerState {
	GENERATED_BODY()
public:
	AUserState();

#pragma region Event Calls
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
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
	UFUNCTION()
		void PlayerDeath();
	UFUNCTION()
		void PlayerDeathProcess();
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
		bool WithdrawItem(FItem transferItem);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool KeepItem(FItem transferItem);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool BuyItem(FItem buyItem);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool SellItem(FItem sellItem);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/*	아이템 착용을 시도합니다. 모듈이 아니거나 함선에 호환되지 않는 등의 문제가 발생할 경우
		*	@param equipItemID - 작용할 아이템의 아이디.
		*	@return - 착용 성공 여부. 모듈이 아니거나 착용조건을 만족하지 못한 경우 false
		*/
		bool EquipModule(const int equipItemID);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool UnEquipModule(const ItemType moduleType, const int slotIndex);

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void SetDockedStructure(const TScriptInterface<IStructureable> dockingStructure);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		void GetDockedStructure(TScriptInterface<IStructureable>& getStructure) const;
	TScriptInterface<IStructureable> DockedStructure() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/*	플레이어의 아이템 데이터를 배열로 획득합니다.
		*	@param setArray - 획득할 아이템 데이터
		*/
		void GetUserDataItem(TArray<FItem>& setArray) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/*	플레이어의 아이템 중 찾고자 하는 아이템의 갯수를 획득합니다.
		*	@param setArray - 획득할 스킬 데이터
		*	@return - 찾는 아이템의 보유 갯수. 만약 찾는 아이템이 존재하지 않는다면 -1을 반환합니다.
		*/
		int FindItemAmount(const int findItemID) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/*	플레이어의 스킬 데이터를 배열로 획득합니다.
		*	@param setArray - 획득할 스킬 데이터
		*/
		void GetUserDataSkill(TArray<FSkillIDANDLevel>& setArray) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/*	현재 수련중인 스킬 데이터를 가져옵니다.
		*	@param learningSkill - 수련중인 스킬 데이터
		*	@return 수련중인 스킬 ID
		*/
		int GetLearningSkill(FSkill& learningSkill) const;
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/*	수련할 스킬을 변경합니다. 기존의 수련은 중단되며, -1을 입력으로 할 경우 StopLearningSkill()과 같습니다.
		*	@param learningId - 수련을 진행할 스킬 ID
		*/
		void SetLearningSkill(const int learningId);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/*	수련을 중지합니다.
		*/
		void StopLearningSkill();
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/*	스킬 습득을 시도합니다.
		*	@param NewLearningSkillId - 습득하고자 하는 스킬의 ID
		*	@return - 스킬 습득 시도에 대한 결과
		*/
		bool NewLearningSkill(const int NewLearningSkillId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		void GetAchievments(TArray<int>& _achievmentsLevels) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		bool CheckSkill(TArray<FSkillIDANDLevel>& checkSkill) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		float CheckCargoValue() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		float CheckAddItemValue(const FItem item) const;

	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool SetRestartLocation();
#pragma endregion

#pragma region Debugging Cheat
	
public:
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
	void CheatCommand(CheatType cheatType, UPARAM(ref) FString& parameters);
#pragma endregion
	
#pragma region Environment Functions & Components & Variables
public:
	/*
	//그래픽 설정 함수
	//사운드 볼륨 설정 컴포넌트 및 변수
	UFUNCTION(BlueprintCallable, Category = "Call Environment Setting Function : State")
		float Ev_ChangeBGMVolume(float volume);
	UFUNCTION(BlueprintCallable, Category = "Call Environment Setting Function : State")
		float Ev_ChangeSfxVolume(float volume);
	//입력 설정 컴포넌트 및 변수
	//게임 플레이 설정 컴포넌트 및 변수

	//그래픽 설정 컴포넌트 및 변수
	bool ev_FullScreenMode;
	FVector2D ev_ScreenResolution;
	int8 ev_GrahpicLevelTextrue;
	int8 ev_GrahpicLevelShader;
	int8 ev_GrahpicLevelShadow;
	int8 ev_GrahpicLevelAntiAliasing;
	int8 ev_GrahpicLevelAnisotropicFiltering;
	bool ev_GrahpicHDR;
	bool ev_GrahpicSSAO;
	//사운드 볼륨 설정 컴포넌트 및 변수
	*/
	UPROPERTY()
		USoundClass* ev_BGMClass;
	UPROPERTY()
		USoundClass* ev_SfxClass;
	UPROPERTY()
		USoundMix* ev_BGMMix;
	UPROPERTY()
		USoundMix* ev_SfxMix;
	/*
	float ev_BGMVolume;
	float ev_SfxVolume;
	//입력 설정 컴포넌트 및 변수
	float ev_KeyAsixSensitivity;
	float ev_MouseAsixSensitivity;
	//게임 플레이 설정 컴포넌트 및 변수
	*/
	/*
	//UI Color
	float ev_UIWndowTopColor;
	float ev_UIBodyTopColor;
	float ev_UIBodyLineColor;
	float ev_UIButtonColor;
	*/
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
	FVector2D restartLocation;
protected:
	int learningSkillId;
	UPROPERTY(BlueprintReadOnly, Category = "User Data")
		TMap<int, FSkill> playerSkill;
	UPROPERTY(BlueprintReadOnly, Category = "User Data")
		TMap<int, int> playerItem;

private:
	FString nextSectorName;
	TScriptInterface<IStructureable> dockedStructure;
	float currentCargo;
	
#pragma endregion

};
