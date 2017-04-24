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
		void AddToObjectList(ASpaceObject* object);
	virtual void AddToObjectList_Implementation(ASpaceObject* object);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void RemoveFromObjectList(ASpaceObject* object);
	virtual void RemoveFromObjectList_Implementation(ASpaceObject* object);
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
		void OnUITarget(ASpaceObject* object, FColor color, float lifeTime);
	virtual void OnUITarget_Implementation(ASpaceObject* object, FColor color, float lifeTime);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnUIMove(FVector location, FColor color, float lifeTime, float size);
	virtual void OnUIMove_Implementation(FVector location, FColor color, float lifeTime, float size);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void OnUIInteraction(AActor* object, ObjectType objectType);
	virtual void OnUIInteraction_Implementation(AActor* object, ObjectType objectType);

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
		void SetUIIOnTopMessage(FText& message, FColor color);
	virtual void SetUIIOnTopMessage_Implementation(FText& message, FColor color);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction CPP to BP")
		void AddUILogMessage(FText& message, FColor color);
	virtual void AddUILogMessage_Implementation(FText& message, FColor color);
#pragma endregion
};
