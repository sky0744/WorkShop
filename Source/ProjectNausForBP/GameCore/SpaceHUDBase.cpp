// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceHUDBase.h"


ASpaceHUDBase::ASpaceHUDBase() {

}

#pragma region UI Update
void ASpaceHUDBase::UpdateUI_Implementation(const UpdateUIType updateType) {
	return;
}

void ASpaceHUDBase::AddToObjectList_Implementation(const ASpaceObject* object) {
	return;
}

void ASpaceHUDBase::RemoveFromObjectList_Implementation(const ASpaceObject* object) {
	return;
}
#pragma endregion

#pragma region On/Off UI
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

void ASpaceHUDBase::OnMobileDoubleTouch_Implementation() {
	return;
}
#pragma endregion

#pragma region Messages
void ASpaceHUDBase::SetUIIOnTopMessage_Implementation(const FText& message, FColor color) {
	return;
}

void ASpaceHUDBase::AddUILogMessage_Implementation(const FText& message, FColor color) {
	return;
}
#pragma endregion