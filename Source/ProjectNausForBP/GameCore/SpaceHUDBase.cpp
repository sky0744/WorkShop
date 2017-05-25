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

void ASpaceHUDBase::AddToObjectList_Implementation(const ASpaceObject* object) {
	return;
}

void ASpaceHUDBase::RemoveFromObjectList_Implementation(const ASpaceObject* object) {
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
void ASpaceHUDBase::TriggerUI_Profile_Implementation() {
	return;
}

void ASpaceHUDBase::TriggerUI_Ship_Implementation() {
	return;
}

void ASpaceHUDBase::TriggerUI_Cargo_Implementation() {
	return;
}

void ASpaceHUDBase::TriggerUI_Contract_Implementation() {
	return;
}

void ASpaceHUDBase::TriggerUI_WorldView_Implementation() {
	return;
}

void ASpaceHUDBase::TriggerUI_Station_Implementation() {
	return;
}

void ASpaceHUDBase::TriggerUI_Menu_Implementation() {
	return;
}

void ASpaceHUDBase::OnUITarget_Implementation(const ASpaceObject* object, const FColor color, const float lifeTime) {
	return;
}

void ASpaceHUDBase::OnUIMove_Implementation(const FVector location, const FColor color, const float lifeTime, const float size) {
	return;
}

void ASpaceHUDBase::OnUIInteraction_Implementation(const AActor* actor, const ObjectType objectType) {
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

#pragma region Response by Controller
void ASpaceHUDBase::OnMobileBack_Implementation() {
	return;
}

void ASpaceHUDBase::OnMobileMenu_Implementation() {
	return;
}
#pragma endregion

#pragma region Messages
void ASpaceHUDBase::SetUIIOnTopMessage_Implementation(FText message, FColor color) {
	return;
}

void ASpaceHUDBase::AddUILogMessage_Implementation(FText message, FColor color) {
	return;
}
#pragma endregion