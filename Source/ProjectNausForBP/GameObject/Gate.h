// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "GameCoreSub/SafeENGINE.h"
#include "SpaceObject.h"
#include "Interface/Structureable.h"
#include "Gate.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API AGate : public ASpaceObject, public IStructureable {
	GENERATED_BODY()
#pragma region Event Calls
public:
	AGate();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void BeginDestroy() override;
#pragma endregion

#pragma region SpaceObject Inheritance
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual int GetObjectID() override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual ObjectType GetObjectType() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual Faction GetFaction() override;
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void SetFaction(Faction setFaction) override;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual BehaviorState GetBehaviorState() override;

	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool InitObject(int objectId) override;
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual bool LoadBaseObject(float shield, float armor, float hull, float power) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to SpaceObject")
		virtual float GetValue(GetStatType statType) override;
	UFUNCTION(BlueprintCallable, Category = "Call to SpaceObject")
		virtual void GetRepaired(GetStatType statType, float repairValue) override;
#pragma endregion

#pragma region Interface Implementation : IStructureable
protected:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual FString GetDestinationName() override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual StructureType GetStationType() override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual bool RequestedDock(Faction requestFaction) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual bool RequestedJump(Faction requestFaction) override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual bool SetStructureData(FStructureInfo& structureData) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Strcutureable")
		virtual void GetStructureData(FStructureInfo& structureData) override;
#pragma endregion

#pragma region Functions
public:
	FStructureInfo* GetStructureDataPointer();
protected:
	void CheckGateRefreshTime();
	void RefreshGateItem();
#pragma endregion

#pragma region Components

#pragma endregion

#pragma region Variables
private:
	FStructureInfo* structureInfo;
	UPROPERTY()
		FString warpDestination;

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