// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectNausForBP.h"
#include "SpaceObject.h"
#include "Resource.generated.h"

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called For SpaceObject")
		virtual int GetObjectID() override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called For SpaceObject")
		virtual ObjectType GetObjectType() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called For SpaceObject")
		virtual Faction GetFaction() override;
	UFUNCTION(BlueprintCallable, Category = "Called For SpaceObject")
		virtual void SetFaction(Faction setFaction) override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called For SpaceObject")
		virtual BehaviorState GetBehaviorState() override;

	UFUNCTION(BlueprintCallable, Category = "Called For SpaceObject")
		virtual bool InitObject(int objectId) override;
	UFUNCTION(BlueprintCallable, Category = "Called For SpaceObject")
		virtual bool LoadBaseObject(float shield, float armor, float hull, float power) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called For SpaceObject")
		virtual float GetValue(GetStatType statType) override;
#pragma endregion

#pragma region Resource Functions
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Call to Resource")
		void SetResource(float durability, FItem ore);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Call to Resource")
		float GetResourceAmount();
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Call to Resource")
		float GetResourceDurability();
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Call to Resource")
		float GetResourceDef();
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Call to Resource")
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
