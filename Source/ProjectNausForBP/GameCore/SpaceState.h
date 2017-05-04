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
		*	@param isRealRelation - true : 실질 관계로 판단, false : 섹터 내 임시 관계로 판단(requestor가 플레이어일 때만 유효)
		*	@return - 식별된 피아
		*/
		Peer PeerIdentify(const Faction requestor, const Faction target, const bool isRealRelation) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		/** 임시 관계/실질 관계와 플레이어 건국 팩션과의 관계성을 동시에 반환합니다.
		*	@param factionRelationship(out) - 주요 팩션과의 관계 배열
		*	@param isRealRelation - true : 실질 관계로 판단, false : 섹터 내 임시 관계로 판단(requestor가 플레이어일 때만 유효)
		*	@return - 플레이어 건국 팩션과의 관계
		*/
		float GetRelationshipArray(TArray<float>& factionRelationship, bool isRealRelation) const;
	/**	가해진 데미지를 팩션의 관계로 변환하여 관계도에 적용. 현재 섹터 내에서 유효
	*	@Param targetFaction - 관계 대상 팩션
	*	@Param damageForConvertRelation - 관계도에 적용할 데미지
	*/
	void ASpaceState::ApplyRelation(const Faction targetFaction, float damageForConvertRelation);
	/**	전략 포인트를 팩션의 관계로 변환하여 관계도에 적용.
	*	@Param targetFaction - 관계 대상 팩션
	*	@Param SPForConvertRelation - 관계도에 적용할 전략 포인트
	*	@Param isRealRelation - true : 실질 관계에만 적용, false : 섹터 내 임시 관계에만 적용
	*/
	void ASpaceState::ApplyRelation(const Faction targetFaction, float SPForConvertRelation, const bool isRealRelation);
#pragma endregion

#pragma region ObjectTMap
private:
	UPROPERTY()
		TArray<FSectorData> sectorInfo;
	FSectorData* currentSectorInfo;
	UPROPERTY()
		UEnum* factionEnumPtr;
	UPROPERTY()
		TArray<float> tempFactionRelationship;
	UPROPERTY()
		TArray<float> factionRelationship;
	float relationwithPlayerEmpire;
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
