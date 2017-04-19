// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "AIController.h"
#include "SpaceAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNAUSFORBP_API ASpaceAIController : public AAIController
{
	GENERATED_BODY()
public:
	ASpaceAIController();
	virtual void Possess(APawn* inPawn) override;

#pragma region AI Command Process

	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		void AICommandStop();
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandMovetoLocation(FVector destination);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandMovetoTarget(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandAttack(ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandMining(TScriptInterface<ICollectable> target);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandRepair(FVector destination);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandJump(TScriptInterface<IStructureable> target);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandWarp(FVector location);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandDock(TScriptInterface<IStructureable> target);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandUndock();
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool AICommandLaunch(TArray<int> baySlot);

	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool OnTargetModule(int slotIndex);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool OffTargetModule(int slotIndex);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool OnActiveModule(int slotIndex);
	UFUNCTION(BlueprintCallable, Category = "AI Controller")
		bool OffActiveModule(int slotIndex);
#pragma endregion

#pragma region Components & Variables
protected:
	UPROPERTY()
		ASpaceObject* possessObj;
	UPROPERTY()
		TScriptInterface<ICommandable> possessCObj;

	UPROPERTY(BlueprintReadOnly, Category = "Space AI")
		UBehaviorTree* aiBehaviorTree;
	UPROPERTY(BlueprintReadOnly, Category = "Space AI")
		UBlackboardComponent* compAIBlackboard;
	UPROPERTY(BlueprintReadOnly, Category = "Space AI")
		UBlackboardData* aiBlackboard;


};