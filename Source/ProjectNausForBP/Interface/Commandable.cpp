// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Commandable.h"

// This function does not need to be modified.
UCommandable::UCommandable(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UFUNCTION(BlueprintCallable, Category = "InterFace : Command")
void ICommandable::CommandStop() {
	unimplemented();
	return;
}

bool ICommandable::CommandMoveToPosition(FVector position) {
	unimplemented();
	return false;
}

bool ICommandable::CommandMoveToTarget(ASpaceObject* target) {
	unimplemented();
	return false;
}

bool ICommandable::CommandAttack(ASpaceObject* target) {
	unimplemented();
	return false;
}

bool ICommandable::CommandMining(AResource* target) {
	unimplemented();
	return false;
}

bool ICommandable::CommandRepair(ASpaceObject* target) {
	unimplemented();
	return false;
}

bool ICommandable::CommandJump(TScriptInterface<IStructureable> target) {
	unimplemented();
	return false;
}

bool ICommandable::CommandWarp(FVector location) {
	unimplemented();
	return false;
}

bool ICommandable::CommandDock(TScriptInterface<IStructureable> target) {
	unimplemented();
	return false;
}

bool ICommandable::CommandUndock() {
	unimplemented();
	return false;
}

bool ICommandable::CommandLaunch(const TArray<int>& baySlot) {
	unimplemented();
	return false;
}
