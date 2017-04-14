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
		virtual void CommandStop();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandMoveToPosition(FVector position);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandMoveToTarget(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandAttack(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandMining(TScriptInterface<ICollectable> target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandRepair(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandJump(TScriptInterface<IStructureable> target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandWarp(FVector location);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandDock(TScriptInterface<IStructureable> target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandUndock();
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandLaunch(TArray<int> baySlot);

	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandToggleTargetModule(int slotIndex, ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
		virtual bool CommandToggleActiveModule(int slotIndex);
};
