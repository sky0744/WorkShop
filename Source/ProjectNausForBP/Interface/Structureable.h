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
		/** ���� ��ũ�� ������ �̸��� ȹ��
		*	@return - ���� ��ũ�� ����
		*/
		virtual FString GetDestinationName();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** ������ ������ ȹ��
		*	@return - �������� ������
		*/
		virtual StructureType GetStationType();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** ��ŷ�� ��û
		*	@param requestFaction - ��ŷ�� ��û�� �Ѽ�
		*	@return - ��ŷ �㰡 ����
		*/
		virtual bool RequestedDock(Faction requestFaction);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** ������ ��û
		*	@param requestFaction - ������ ��û�� �Ѽ�
		*	@return - ���� �㰡 ����
		*/
		virtual bool RequestedJump(Faction requestFaction);

	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** ������ �����͸� �ʱ�ȭ
		*	@param structureData - �ʱ�ȭ �� ������ ������
		*	@return - ������ �ʱ�ȭ ���
		*/
		virtual bool SetStructureData(FStructureInfo& structureData);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Structureable")
		/** ������ �����͸� ȹ��
		*	@param structureData - ȹ���� ������ ������(out)
		*/
		virtual void GetStructureData(FStructureInfo& structureData);
};
