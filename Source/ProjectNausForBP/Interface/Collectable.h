// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ProjectNausForBP.h"
#include "Collectable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCollectable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class PROJECTNAUSFORBP_API ICollectable
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "InterFace : Collect")
		/** Collectable - ���� ����
		*	@param durability - ���� ������
		*	@param ore - ���� ������
		*/
		virtual void SetResource(float durability, FItem ore);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Collect")
		/** Collectable - ���� �������� ����
		*	@return - ���� ������
		*/
		virtual float GetResourceAmount();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Collect")
		/** Collectable - ���� �������� ����
		*	@return - ���� ������
		*/
		virtual float GetResourceDurability();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Collect")
		/** Collectable - ���� ������ ����
		*	@return - ���� ����
		*/
		virtual float GetResourceDef();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Collect")
		/** Collectable - ä��
		*	@param miningPerfomance - ä�� ����
		*	@return - ä���� ����
		*/
		virtual FItem CollectResource(float miningPerfomance);
};
