// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ProjectNausForBP.h"
#include "SpaceHUDBase.h"
#include "../GameCoreSub/Enums.h"
#include "../GameObject/PlayerShip.h"
#include "UserController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNAUSFORBP_API AUserController : public APlayerController {
	GENERATED_BODY()
public:
	AUserController();
#pragma region Event Calls
protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;
#pragma endregion

#pragma region Input Binding
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ControlCamX(float value);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ControlCamY(float value);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ControlMouseWheel(float value);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ControlCamReset();

	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void ControlTargetSpeed(float value);
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void ControlAcceleration(float value);
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void ControlRotateSpeed(float value);

	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void OpenInfoProfile();
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void OpenInfoShip();
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void OpenInfoItem();
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void OpenInfoMap();
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void OpenInfoQuest();
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void OpenInfoMenu();
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void KeyUndock();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void Touch1Press(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void Touch1Release(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void Touch2Press(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void Touch2Release(FKey key);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ClickPressMouseLeft(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ClickReleaseMouseLeft(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ClickPressMouseWheel(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ClickReleaseMouseWheel(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ClickPressMouseRight(FKey key);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void ClickReleaseMouseRight(FKey key);
#pragma endregion

#pragma region player flow control
public:
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void PlayerInterAction(const InteractionType interaction);

	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		bool ToggleTargetModule(const int slotIndex);
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		bool ToggleActiveModule(const int slotIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Player Controller")
		ASpaceObject* GetTargetInfo();
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void SetTarget(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void SettingInteraction(const ASpaceObject* target);
#pragma endregion

#pragma region Components & Variables
private:
	UPROPERTY()
		ASpaceHUDBase* controlledHUD;
	UPROPERTY()
		APlayerShip* controlledPawn;
	UPROPERTY()
		TScriptInterface<ICommandable> commandInterface;
	UPROPERTY()
		ASpaceObject* tObj;

	FVector mousePositionInWorld;
	FVector mouseDirectionInWorld;
	FVector mouseEndPositionInWorld;
	FVector mouseXYPlane;
	bool mouseRightClicked;
	bool mouseLeftClicked;

	FCollisionObjectQueryParams traceObjectParams;
	FCollisionQueryParams traceParams;
	FHitResult hitResult;
	FVector warpLocation;
#pragma endregion
};
