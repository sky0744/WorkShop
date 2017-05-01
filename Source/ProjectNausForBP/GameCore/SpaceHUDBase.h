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
		void UpdateUIUser();
	virtual void UpdateUIUser_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void UpdateUIShip();
	virtual void UpdateUIShip_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void UpdateUIModule();
	virtual void UpdateUIModule_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void UpdateUICargo();
	virtual void UpdateUICargo_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void UpdateUIContract();
	virtual void UpdateUIContract_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void AddToObjectList(const ASpaceObject* object);
	virtual void AddToObjectList_Implementation(const  ASpaceObject* object);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void RemoveFromObjectList(const  ASpaceObject* object);
	virtual void RemoveFromObjectList_Implementation(const  ASpaceObject* object);
#pragma endregion

#pragma region Docked UI Update
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void UpdateUIStationOverTime();
	virtual void UpdateUIStationOverTime_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void UpdateUIStationOnRequest();
	virtual void UpdateUIStationOnRequest_Implementation();
#pragma endregion

#pragma region On/Off UI
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void TriggerUI_Profile();
	virtual void TriggerUI_Profile_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void TriggerUI_Ship();
	virtual void TriggerUI_Ship_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void TriggerUI_Cargo();
	virtual void TriggerUI_Cargo_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void TriggerUI_Contract();
	virtual void TriggerUI_Contract_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void TriggerUI_WorldView();
	virtual void TriggerUI_WorldView_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void TriggerUI_Station();
	virtual void TriggerUI_Station_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void TriggerUI_Menu();
	virtual void TriggerUI_Menu_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnUITarget(const ASpaceObject* object, const FColor color, const float lifeTime);
	virtual void OnUITarget_Implementation(const ASpaceObject* object, const FColor color, const float lifeTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnUIMove(const FVector location, const FColor color, const float lifeTime, const float size);
	virtual void OnUIMove_Implementation(const FVector location, const FColor color, const float lifeTime, const float size);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnUIInteraction(const AActor* object, const ObjectType objectType);
	virtual void OnUIInteraction_Implementation(const AActor* object, const ObjectType objectType);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OffUIInteraction();
	virtual void OffUIInteraction_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnUIStationButton();
	virtual void OnUIStationButton_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OffUIStationButton();
	virtual void OffUIStationButton_Implementation();
#pragma endregion

#pragma region Messages
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void SetUIIOnTopMessage(const FText& message, FColor color);
	virtual void SetUIIOnTopMessage_Implementation(FText message, FColor color);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void AddUILogMessage(const FText& message, FColor color);
	virtual void AddUILogMessage_Implementation(FText message, FColor color);
#pragma endregion
};
