// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "SpaceObject.h"
#include "../Interface/Structureable.h"
#include "Station.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API AStation : public ASpaceObject, public IStructureable {
	GENERATED_BODY()
#pragma region Event Call
public:
	AStation();
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
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void GetRepaired(const GetStatType statType, float repairValue) override;
#pragma endregion

#pragma region Interface Implementation : IStructureable
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual const FString GetDestinationName() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual StructureType GetStationType() const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual bool RequestedDock(const Faction requestFaction) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual bool RequestedJump(const Faction requestFaction) const override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual bool SetStructureData(UPARAM(ref) FStructureInfo& structureData) override;
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual void GetStructureData(FStructureInfo& structureData) const override;
#pragma endregion

#pragma region Functions
public:
	FStructureInfo* GetStructureDataPointer() const;
#pragma endregion

#pragma region Components

#pragma endregion

#pragma region Variables
protected:
	FStructureInfo* structureInfo;
	bool isInited;

	float maxShield;
	float rechargeShield;
	float currentShield;
	float defShield;

	float maxArmor;
	float repairArmor;
	float currentArmor;
	float defArmor;

	float maxHull;
	float repairHull;
	float currentHull;
	float defHull;
#pragma endregion
};