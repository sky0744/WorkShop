// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Structureable.h"


// This function does not need to be modified.
UStructureable::UStructureable(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

// Add default functionality here for any IStructureable functions that are not pure virtual.

const FString IStructureable::GetDestinationName() const {
	unimplemented();
	return "";
}

StructureType IStructureable::GetStationType() const {
	unimplemented();
	return StructureType::TradingCenter;
}

bool IStructureable::RequestedDock(const Faction requestFaction, const ShipClass requestClass, FDockSlot*& dockSlotData, FVector dockLocation) {
	unimplemented();
	return false;
}

bool IStructureable::RequestedJump(const Faction requestFaction) const {
	unimplemented();
	return false;
}


bool IStructureable::SetStructureData(UPARAM(ref) FStructureInfo& structureData) {
	unimplemented();
	return false;
}

void IStructureable::GetStructureData(FStructureInfo& structureData) const {
	unimplemented();
	return;
}