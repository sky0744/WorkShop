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
		virtual const FString GetDestinationName() const;
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 구조물 타입 획득
		*	@return - 구조물 타입
		*/
		virtual StructureType GetStationType() const;
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 스테이션에 도킹을 요청합니다.
		*	@param requestFaction - 도킹을 요청한 팩션
		*   @param requestFaction - 도킹을 요청한 팩션
		*	@param out dockSlotData - 도킹이 허가된 슬롯
		*	@return - 도킹 허가 여부
		*/
		virtual bool RequestedDock(const Faction requestFaction, const ShipClass requestClass, FVector& slotLocation, FRotator& slotRotation);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 점프를 요청
		*	@param requestFaction - 점프를 요청한 팩션
		*	@return - 점프 허가 여부
		*/
		virtual bool RequestedJump(const Faction requestFaction) const;

	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 구조물 데이터를 초기화
		*	@param structureData - 초기화 할 구조물 데이터
		*	@return - 구조물 초기화 결과
		*/
		virtual bool SetStructureData(UPARAM(ref) FStructureInfo& structureData);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** 구조물 데이터를 획득
		*	@param structureData - 획득할 구조물 데이터(out)
		*/
		virtual void GetStructureData(FStructureInfo& structureData) const;

	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/*	구조물의 아이템 데이터를 배열로 획득합니다. 정렬 옵션이 가능합니다.
		*   옵션을 입력하지 않거나 NoOption을 입력으로 하면 정렬하지 않은 상태로 반환합니다.
		*	@param setArray - 획득할 아이템 데이터
		*	@param isPlayerCargo - 찾고자 하는 슬롯이 플레이어의 보관슬롯이면 true, 스테이션 소유의 슬롯이면 false
		*	@param sortType - 정렬 옵션
		*/
		virtual void GetStructureCargo(TArray<FItem>& setArray, bool isPlayerCargo, CargoSortType sortType = CargoSortType::NoOption) const;
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		/*	쿠조물의 아이템 중 찾고자 하는 아이템의 갯수를 획득합니다.
		*	@param findItemID - 찾고자 하는 아이템의 ID
		*	@param isPlayerCargo - 찾고자 하는 슬롯이 플레이어의 보관슬롯이면 true, 스테이션 소유의 슬롯이면 false
		*	@return - 찾는 아이템의 보유 갯수. 만약 찾는 아이템이 존재하지 않는다면 -1을 반환합니다.
		*/
		virtual int FindStructureCargoAmount(const int findItemID, bool isPlayerCargo) const;
};
