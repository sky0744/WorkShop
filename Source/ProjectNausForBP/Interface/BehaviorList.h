// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "BehaviorList.generated.h"


USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FBehaviorBlock
{
	GENERATED_USTRUCT_BODY()
public:
	FBehaviorBlock(BehaviorState state = BehaviorState::Idle, ASpaceObject* actor = nullptr, float distance = 0.0f, FVector location = FVector())
		: objectState(state)
		, targetActor(actor)
		, targetDistance(distance)
		, targetLocation(location) {}

	UPROPERTY(BlueprintReadWrite, Category = "Behavior List")
		BehaviorState objectState;
	UPROPERTY(BlueprintReadWrite, Category = "Behavior List")
		ASpaceObject* targetActor;
	UPROPERTY(BlueprintReadWrite, Category = "Behavior List")
		float targetDistance;
	UPROPERTY(BlueprintReadWrite, Category = "Behavior List")
		FVector targetLocation;
};


UCLASS(Blueprintable, BlueprintType)
class PROJECTNAUSFORBP_API UBehaviorList : public UObject
{
	GENERATED_BODY()
public:
	UBehaviorList();
	UPROPERTY(BlueprintReadWrite, Category = "Behavior List")
		TArray<FBehaviorBlock> behaviorBlockList;
	UPROPERTY(BlueprintReadWrite, Category = "Behavior List")
		TArray<FItem> tradeItemList;
	UPROPERTY(BlueprintReadWrite, Category = "Behavior List")
		bool isCarryingSupplement;
};
