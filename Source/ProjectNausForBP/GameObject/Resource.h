// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectNausForBP.h"
#include "SpaceObject.h"
#include "Resource.generated.h"

#pragma region Constant Value Resource Type - Backup / Real
/*
const float _define_StatDamperMIN = 10.0f;
const float _define_StatDamperMAX = 1000000.0f;
const float _define_StatDefMIN = -1000.0f;
const float _define_StatDefMAX = 1000.0f;

const float _define_DamagedMin = 1.0f;
const float _define_DamagedMAX = 10000.0f;
*/
const float _define_StatResourceAmountMIN = 0.0f;
const float _define_StatResourceAmountMAX = 10000.0f;
#pragma endregion

UCLASS()
class PROJECTNAUSFORBP_API AResource : public ASpaceObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AResource();

#pragma region Event Calls
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void BeginDestroy() override;
#pragma endregion

#pragma region SpaceObject Inheritance
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual int GetObjectID() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual ObjectType GetObjectType() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual Faction GetFaction() const override;
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void SetFaction(const Faction setFaction) override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual BehaviorState GetBehaviorState() const override;

	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool InitObject(const int objectId) override;
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool LoadBaseObject(const float shield, const float armor, const float hull, const float power) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual float GetValue(const GetStatType statType) const override;
#pragma endregion

#pragma region Resource Functions
public:
	UFUNCTION(BlueprintCallable, Category = "Call to Resource")
		void SetResource(const int resourceId, float durability, int amount);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to Resource")
		float GetResourceAmount() const;
	UFUNCTION(BlueprintCallable, Category = "Call to Resource")
		FItem CollectResource(float miningPerfomance);
#pragma endregion

#pragma region Components

#pragma endregion

#pragma region Values
private:
	float resourceID;

	ResourceType resourceType;
	float lengthToLongAsix;

	FItem currentResource;
	float defResource;
#pragma endregion
};
