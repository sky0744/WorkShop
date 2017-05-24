// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Collectable.h"

// This function does not need to be modified.
UCollectable::UCollectable(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

// Add default functionality here for any ICollectable functions that are not pure virtual 


void ICollectable::SetResource(float durability, FItem ore) {
	unimplemented();
	return;
}

float ICollectable::GetResourceAmount() {
	unimplemented();
	return -1.0f;
}

float ICollectable::GetResourceDurability() {
	unimplemented();
	return -1.0f;
}

float ICollectable::GetResourceDef() {
	unimplemented();
	return -1.0f;
}

FItem ICollectable::CollectResource(float miningPerfomance) {
	unimplemented();
	return FItem();
}
