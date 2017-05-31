// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "ProjectNausForBP.h"
#include "../GameCoreSub/Enums.h"
#include "SpaceHUDBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNAUSFORBP_API ASpaceHUDBase : public AHUD
{
	GENERATED_BODY()
public:
		ASpaceHUDBase();

#pragma region UI Update
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void UpdateUI(const UpdateUIType updateType);
	virtual void UpdateUI_Implementation(const UpdateUIType updateType);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void AddToObjectList(const ASpaceObject* object);
	virtual void AddToObjectList_Implementation(const  ASpaceObject* object);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void RemoveFromObjectList(const  ASpaceObject* object);
	virtual void RemoveFromObjectList_Implementation(const  ASpaceObject* object);
#pragma endregion

#pragma region On/Off UI
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnUIStationButton();
	virtual void OnUIStationButton_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OffUIStationButton();
	virtual void OffUIStationButton_Implementation();
#pragma endregion

#pragma region Response by Controller
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnMobileBack();
	virtual void OnMobileBack_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnMobileMenu();
	virtual void OnMobileMenu_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnMobileDoubleTouch();
	virtual void OnMobileDoubleTouch_Implementation();
#pragma endregion

#pragma region Messages
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void SetUIOnTopMessage(const FText& message, FColor color);
	virtual void SetUIOnTopMessage_Implementation(const FText& message, FColor color);
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void SetUIOnTopMessageToString(const FString& message, FColor color);
	virtual void SetUIOnTopMessageToString_Implementation(const FString& message, FColor color);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void AddUILogMessage(const FText& message, MessageLogType messageType, FColor color);
	virtual void AddUILogMessage_Implementation(const FText& message, MessageLogType messageType, FColor color);
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void AddUILogMessageToString(const FString& message, MessageLogType messageType, FColor color);
	virtual void AddUILogMessageToString_Implementation(const FString& message, MessageLogType messageType,FColor color);
#pragma endregion
};
