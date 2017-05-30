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
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void ControlTargetSpeed(float value);
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void ControlAcceleration(float value);
	UFUNCTION(BlueprintCallable, Category = "Player Controller")
		void ControlRotateSpeed(float value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void RepeatTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void TouchBack();
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Player Controller")
		void TouchMenu();
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

	FVector touchPositionInWorld;
	FVector touchDirectionInWorld;
	FVector touchEndPositionInWorld;
	FVector touchXYPlane;
	bool isMultiTouching;
	FVector2D originPositionTouch1;
	FVector2D originPositionTouch2;
	
	FCollisionObjectQueryParams traceObjectParams;
	FCollisionQueryParams traceParams;
	FHitResult hitResult;
	FVector warpLocation;
#pragma endregion
};
