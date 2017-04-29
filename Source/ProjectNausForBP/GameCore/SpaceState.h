// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"

#include "ProjectNausForBP.h"
#include "../GameCoreSub/SafeENGINE.h"
#include "../GameCoreSub/SaveLoader.h"

#include "../Interface/Commandable.h"
#include "../Interface/Structureable.h"

#include "../GameCoreSub/Enums.h"
#include "SpaceState.generated.h"

#pragma region Constant Value In SpaceState - Real
const float _define_FactionRelationshipMIN = -10.0f;
const float _define_FactionRelationshipMAX = 10.0f;

const float _define_RelationThresholdAllyStrong = 8.0f;
const float _define_RelationThresholdAlly = 5.0f;
const float _define_RelationThresholdFriend = 2.0f;
const float _define_RelationThresholdBoundary = -2.0f;
const float _define_RelationThresholdEnemy = -5.0f;
const float _define_RelationThresholdEnemyStrong = -8.0f;

const float _define_DamagetoRelationFactorMIN = 0.01f;
const float _define_DamagetoRelationFactorMAX = 0.02f;
const float _define_SPtoRelationFactorMIN = 0.002f;
const float _define_SPtoRelationFactorMAX = 0.005f;
#pragma endregion

#pragma region Constant Value In SpaceState - Not Use Now
//const float _define_TransAllyRelationship = 0.6f;
//const float _define_TransSubHostileRelationship = -0.15f;
//const float _define_TransHostileRelationship = -0.7f;
#pragma endregion


UCLASS()
class PROJECTNAUSFORBP_API ASpaceState : public AGameStateBase {
	GENERATED_BODY()
public:
	ASpaceState();
#pragma region Event Call
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
#pragma endregion

#pragma region Save/Load
public:
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool SaveSpaceState(USaveLoader* saver);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		bool LoadSpaceState(USaveLoader* loader);
#pragma endregion

#pragma region Get Data Or SectorData Processing
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		void GetCurrentSectorInfo(FSectorData& getSectorInfo);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		bool isValidSector(const FString& checkSectorName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/** 임시 관계/실질 관계를 기준으로 팩션간의 피아를 반환합니다.
		*	@param requestor - 피아 식별 요청 팩션
		*	@param target - 식별 대상 팩션
		*	@param isRealRelation - true : 실질 관계로 판단, false : 섹터 내 임시 관계로 판단
		*	@return - 식별된 피아
		*/
		Peer PeerIdentify(const Faction requestor, const Faction target, const bool isRealRelation) const;
	/** 타격을 받은 오브젝트에 대한 데미지를 변환하여 팩션간의 임시 관계 변화에 반영합니다.
	*	@param requestor - 관계 변화를 요구하는 팩션
	*	@param target - 관계 변화 대상 팩션
	*	@param damagePoint - 전략 포인트.
	*/
	void ChangeRelationship(const Faction requestor, const Faction target, const float damagePoint);
	/** 전투 결과, 파괴된 오브젝트의 전략 포인트를 변환하여 팩션간의 임시 관계/실질 관계 변화에 반영합니다.
	*	@param requestor - 관계 변화를 요구하는 팩션
	*	@param target - 관계 변화 대상 팩션
	*	@param isRealRelation - true : 실질/섹터 내 임기 관계에 모두 적용, false : 섹터 내 임시 관계에만 적용
	*	@param strategicPoint - 전략 포인트.
	*/
	void ChangeRelationship(const Faction requestor, const Faction target, const bool isRealRelation, const float strategicPoint);
private:
	/**	팩션의 관계도 처리를 상호 적용. 섹터 내 임시 관계도에 영향.(이벤트, 퀘스트 트리거 등)
	*	@Param applyFaction1 - 관계 대상 팩션 1
	*	@Param applyFaction2 - 관계 대상 팩션 2
	*	@Param relation - 적용할 관계성
	*/
	void InterApplyRelation(const Faction applyFaction1, const Faction applyFaction2, float relation);
	/**	팩션의 관계도 처리를 상호 적용.
	*	@Param applyFaction1 - 관계 대상 팩션 1
	*	@Param applyFaction2 - 관계 대상 팩션 2
	*	@Param isRealRelation - true : 실질 관계에만 적용, false : 섹터 내 임시 관계에만 적용
	*	@Param relation - 적용할 관계성
	*/
	void InterApplyRelation(const Faction applyFaction1, const Faction applyFaction2, const bool isRealRelation, float relation);
#pragma endregion

#pragma region ObjectTMap
private:
	UPROPERTY()
		TArray<FSectorData> sectorInfo;
	FSectorData* currentSectorInfo;
	UPROPERTY()
		UEnum* factionEnumPtr;
	UPROPERTY()
		TArray<FFactionRelationship> realFactionRelationship;
	UPROPERTY()
		TArray<FFactionRelationship> tempFactionRelationship;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data In State")
		FText playerFactionName;
#pragma endregion

#pragma region Respawn Ship
public:
	void AccumulateToShipCapacity(bool isDestroying);
private:
	int shipRegenLimit;
	int currentShipCapacity;
	bool skipingFirstTick;

	int totalChanceFactor;
	int shipRegenAmount;
	int randomResultForRegen;
	int randomGateForRegenLocation;
};
