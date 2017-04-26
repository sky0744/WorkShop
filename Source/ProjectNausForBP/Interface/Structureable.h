// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ProjectNausForBP.h"
#include "Structureable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UStructureable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class PROJECTNAUSFORBP_API IStructureable
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 점프 링크된 섹터의 이름을 획득
		*	@return - 점프 링크된 섹터
		*/
		virtual FString GetDestinationName();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 구조물 데이터 획득
		*	@return - 구조물의 데이터
		*/
		virtual StructureType GetStationType();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 도킹을 요청
		*	@param requestFaction - 도킹을 요청한 팩션
		*	@return - 도킹 허가 여부
		*/
		virtual bool RequestedDock(Faction requestFaction);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 점프를 요청
		*	@param requestFaction - 점프를 요청한 팩션
		*	@return - 점프 허가 여부
		*/
		virtual bool RequestedJump(Faction requestFaction);

	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 구조물 데이터를 초기화
		*	@param structureData - 초기화 할 구조물 데이터
		*	@return - 구조물 초기화 결과
		*/
		virtual bool SetStructureData(FStructureInfo& structureData);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 구조물 데이터를 획득
		*	@param structureData - 획득할 구조물 데이터(out)
		*/
		virtual void GetStructureData(FStructureInfo& structureData);
};
