// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Structureable.h"


// This function does not need to be modified.
UStructureable::UStructureable(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

// Add default functionality here for any IStructureable functions that are not pure virtual.

FString IStructureable::GetDestinationName() {
	unimplemented();
	return "";
}

StructureType IStructureable::GetStationType() {
	unimplemented();
	return StructureType::TradingCenter;
}

bool IStructureable::RequestedDock(Faction requestFaction) {
	unimplemented();
	return false;
}

bool IStructureable::RequestedJump(Faction requestFaction) {
	unimplemented();
	return false;
}


bool IStructureable::SetStructureData(FStructureInfo& structureData) {
	unimplemented();
	return false;
}

void IStructureable::GetStructureData(FStructureInfo& structureData) {
	unimplemented();
	return;
}