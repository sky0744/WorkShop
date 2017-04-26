// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ProjectNausForBP.h"
#include "Commandable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCommandable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class PROJECTNAUSFORBP_API ICommandable
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 정지 명령
		*/
		virtual void CommandStop();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 도킹을 요청
		*	@param position - 목표 위치
		*	@return - 이동 가능 여부
		*/
		virtual bool CommandMoveToPosition(FVector position);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 추적 명령
		*	@param target - 추적 대상
		*	@return - 이동 가능 여부
		*/
		virtual bool CommandMoveToTarget(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 공격 명령
		*	@param target - 공격 대상
		*	@return - 공격 가능 여부
		*/
		virtual bool CommandAttack(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 채광 명령
		*	@param target - 채광 대상
		*	@return - 채광 가능 여부
		*/
		virtual bool CommandMining(AResource* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 수리 명령
		*	@param target - 수리 대상
		*	@return - 수리 가능 여부
		*/
		virtual bool CommandRepair(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 점프 명령
		*	@param target - 점프를 시도할 구조물
		*	@return - 점프 가능 여부
		*/
		virtual bool CommandJump(TScriptInterface<IStructureable> target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 워프 명령
		*	@param location - 워프를 시도할 목표 지점
		*	@return - 워프 가능 여부
		*/
		virtual bool CommandWarp(FVector location);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 도킹 명령
		*	@param target - 도킹을 시도할 구조물
		*	@return - 도킹 가능 여부
		*/
		virtual bool CommandDock(TScriptInterface<IStructureable> target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 언도킹 명령
		*	@return - 언도킹 성공 여부
		*/
		virtual bool CommandUndock();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		/** 도킹을 요청
		*	@param baySlot - 런칭할 슬롯 인덱스의 배열
		*	@return - 런칭 성공 여부
		*/
		virtual bool CommandLaunch(TArray<int> baySlot);
};
