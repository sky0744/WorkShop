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

/**
 * 
 */
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
		*	@param isRealRelation - true : 실질 관계로 판단, false : 임시 관계로 판단
		*	@return - 식별된 피아
		*/
		Peer PeerIdentify(const Faction requestor, const Faction target, const bool isRealRelation) const;
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/** 팩션간의 임시 관계/실질 관계 변화를 지정합니다.
		*	@param requestor : 관계 변화를 요구하는 팩션
		*	@param target : 관계 변화 대상 팩션
		*	@param isRealRelation - true : 실질 관계에 적용, false : 임시 관계에 적용
		*	@param varianceValue : 관계 변화율. 범위는 -100.0f ~ 100.0f
		*/
		void ChangeRelationship(const Faction requestor, const Faction target, const bool isRealRelation, const float varianceValue);
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
