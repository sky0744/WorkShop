// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "SpaceObject.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API ASpaceObject : public APawn
{
	GENERATED_BODY()
#pragma region Event Call
public:
	ASpaceObject();
protected:
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void BeginDestroy() override;
#pragma endregion

#pragma region SpaceObject Inheritance
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual int GetObjectID();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual ObjectType GetObjectType();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual Faction GetFaction();
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void SetFaction(Faction setFaction);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual BehaviorState GetBehaviorState();

	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool InitObject(int objectId);
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool LoadBaseObject(float shield, float armor, float hull, float power);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual float GetValue(GetStatType statType);
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void GetRepaired(GetStatType statType, float repairValue);
#pragma endregion

#pragma region Components
protected:
	UPROPERTY(VisibleAnyWhere, Category = "Space Object Data")
		UStaticMeshComponent* objectMesh;
#pragma endregion

#pragma region Variables
protected:
		int objectID;
		Faction faction;
		float lengthToLongAsix;

		float maxDurability;
		float currentDurability;
#pragma endregion
};

