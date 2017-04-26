// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceHUDBase.h"


ASpaceHUDBase::ASpaceHUDBase() {

}

#pragma region UI Update
void ASpaceHUDBase::UpdateUIUser_Implementation() {
	return;
}

void ASpaceHUDBase::UpdateUIShip_Implementation() {
	return;
}

void ASpaceHUDBase::UpdateUIModule_Implementation() {
	return;
}

void ASpaceHUDBase::UpdateUICargo_Implementation() {
	return;
}

void ASpaceHUDBase::UpdateUIContract_Implementation() {
	return;
}

void ASpaceHUDBase::AddToObjectList_Implementation(ASpaceObject* object) {
	return;
}

void ASpaceHUDBase::RemoveFromObjectList_Implementation(ASpaceObject* object) {
	return;
}
#pragma endregion

#pragma region Docked UI Update
void ASpaceHUDBase::UpdateUIStationOverTime_Implementation() {
	return;
}

void ASpaceHUDBase::UpdateUIStationOnRequest_Implementation() {
	return;
}
#pragma endregion

#pragma region On/Off UI
void ASpaceHUDBase::OnUITarget_Implementation(ASpaceObject* object, FColor color, float lifeTime) {
	return;
}

void ASpaceHUDBase::OnUIMove_Implementation(FVector location, FColor color, float lifeTime, float size) {
	return;
}

void ASpaceHUDBase::OnUIInteraction_Implementation(AActor* actor, ObjectType objectType) {
	return;
}

void ASpaceHUDBase::OffUIInteraction_Implementation() {
	return;
}

void ASpaceHUDBase::OnUIStationButton_Implementation() {
	return;
}

void ASpaceHUDBase::OffUIStationButton_Implementation() {
	return;
}
#pragma endregion

#pragma region Messages
void ASpaceHUDBase::SetUIIOnTopMessage_Implementation(FText& message, FColor color) {
	return;
}

void ASpaceHUDBase::AddUILogMessage_Implementation(FText& message, FColor color) {
	return;
}
#pragma endregion