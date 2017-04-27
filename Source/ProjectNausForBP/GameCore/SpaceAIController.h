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

#pragma region Components & Variables
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Space AI")
		UBehaviorTree* aiBehaviorTree;
	UPROPERTY(BlueprintReadOnly, Category = "Space AI")
		UBlackboardComponent* compAIBlackboard;
	UPROPERTY(BlueprintReadOnly, Category = "Space AI")
		UBlackboardData* aiBlackboard;
};
