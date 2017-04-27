// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "SpaceObject.generated.h"

#pragma region Constant Value SpaceObject Type
const float _define_StatDamperMIN = 10.0f;
const float _define_StatDamperMAX = 1000000.0f;
const float _define_StatRestoreMIN = 0.0f;
const float _define_StatRestoreMAX = 500.0f;
const float _define_StatDefMIN = -1000.0f;
const float _define_StatDefMAX = 1000.0f;

const float _define_StatSpeedMIN = 0.0f;
const float _define_StatSpeedMAX = 10000.0f;
const float _define_StatAccelMIN = 0.0f;
const float _define_StatAccelMAX = 10000.0f;
const float _define_StatRotateMIN = 0.0f;
const float _define_StatRotateMAX = 90.0f;

const float _define_StatBonusMIN = 0.0f;
const float _define_StatBonusMAX = 5.0f;
const float _define_StatDroneControlMIN = 0.0f;
const float _define_StatDroneControlMAX = 20.0f;

const float _define_DamageMin = 1.0f;
const float _define_DamageMAX = 10000.0f;
#pragma endregion

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
		virtual int GetObjectID() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual void GetObjectName(FText& spaceObjectName) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual ObjectType GetObjectType() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual Faction GetFaction() const;
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void SetFaction(const Faction setFaction);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual BehaviorState GetBehaviorState() const;

	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool InitObject(const int objectId);
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool LoadBaseObject(const float shield, const float armor, const float hull, const float power);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual float GetValue(const GetStatType statType) const;
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void GetRepaired(const GetStatType statType, float repairValue);
#pragma endregion

#pragma region Components
protected:
	UPROPERTY(VisibleAnyWhere, Category = "Space Object Data")
		UStaticMeshComponent* objectMesh;
#pragma endregion

#pragma region Variables
protected:
		int objectID;
		UPROPERTY()
			FText objectName;
		Faction faction;
		float lengthToLongAsix;

		float maxDurability;
		float currentDurability;
#pragma endregion
};

