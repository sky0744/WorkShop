// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"

#include "SpaceObject.h"
#include "../Interface/Commandable.h"
#include "PlayerShip.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API APlayerShip : public ASpaceObject, public ICommandable
{
	GENERATED_BODY()
#pragma region Event Call
public:
	APlayerShip();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* inputComponent) override;
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

#pragma region Funcations : PlayerShip Only
protected:
	bool TotalStatsUpdate();
	void CheckPassiveTypeModule(const BonusStatType type, float value);
	void CheckBonusStat(const TArray<FBonusStat>& bonusStatArray);
public:
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual bool LoadFromSave(const USaveLoader* loader);

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual bool EquipModule(const int moduleID);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual bool UnEquipModule(const ItemType moduleItemType, const int slotNumber);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to Player Ship")
		virtual void GetModule(ItemType moduleType, TArray<int>& moduleList) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to Player Ship")
		virtual void GetModuleActivate(const ItemType moduleType, TArray<float>& moduleActivate) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to Player Ship")
		virtual void GetTargetModuleAmmo(TArray<FItem>& targetModuleAmmo) const;

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void SetTargetSpeed(const float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void SetAcceleration(const float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void SetRotateRate(const float value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Call to Player Ship")
		virtual bool ToggleTargetModule(const int slotIndex, ASpaceObject* target);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void SettingAmmo(const int selectedAmmoID);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Call to Player Ship")
		virtual bool ToggleActiveModule(const int slotIndex);
#pragma endregion

#pragma region Interface Implementation : ICommandable
protected:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual void CommandStop() override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandMoveToPosition(FVector position) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandMoveToTarget(ASpaceObject* target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandAttack(ASpaceObject* target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandMining(AResource* target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandRepair(ASpaceObject* target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandJump(TScriptInterface<IStructureable> target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandWarp(FVector location) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandDock(TScriptInterface<IStructureable> target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandUndock() override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandLaunch(const TArray<int>& baySlot) override;
#pragma endregion

#pragma region Functions
private:
	bool MoveDistanceCheck();
	void RotateCheck();
	void Movement();
	void ModuleCheck();

	bool CheckCanBehavior() const;

	const float CalculateCompute() const;
	const float CalculatePowerGrid() const;
#pragma endregion

#pragma region Path Finder
private:
	void CheckPath();
	void RequestPathUpdate();
	
	UPROPERTY()
		TArray<FVector> wayPoint;


	//성능 테스트를 위해 전역변수 <-> 지역변수 스트레스 테스트 할 것임
	UPROPERTY()
		UNavigationPath* waypointData;
	
	FCollisionObjectQueryParams traceObjectParams;
	FCollisionQueryParams traceParams;
	FHitResult pathFindTraceResult;
	TArray<FHitResult> frontTraceResult;
	float checkTime;
	float tempDeltaTime;

	FVector moveTargetVector;
	FVector targetVector;
	FVector realMoveFactor;
	FRotator targetRotate;

	int currentClosedPathIndex;
	float remainDistance;
	bool bIsStraightMove;
	bool bIsFirstCheckStraight;
	float nextPointDistance;
	float nextPointOuter;
	//여기까지
#pragma endregion

#pragma region Components
private:
	UPROPERTY(VisibleAnyWhere, Category = "Ship Data")
		UFloatingPawnMovement* objectMovement;
#pragma endregion

#pragma region Variables
private:
	int sShipID;
	bool sIsInited;

	ShipClass shipClass;
	BehaviorState behaviorState;

	float lengthToLongAsix;
	float lengthRadarRange;
	float lengthWeaponRange;

	UPROPERTY()
		ASpaceObject* targetObject;
	UPROPERTY()
		TScriptInterface<ICommandable> targetCommand;
	UPROPERTY()
		TScriptInterface<IStructureable> targetStructure;

	float sMaxShield;
	float sRechargeShield;
	float sCurrentShield;
	float sDefShield;

	float sMaxArmor;
	float sRepairArmor;
	float sCurrentArmor;
	float sDefArmor;

	float sMaxHull;
	float sRepairHull;
	float sCurrentHull;
	float sDefHull;

	float sMaxPower;
	float sRechargePower;
	float sCurrentPower;

	UPROPERTY()
		TArray<FTargetModule> slotTargetModule;
	UPROPERTY()
		TArray<FActiveModule> slotActiveModule;
	UPROPERTY()
		TArray<int> slotPassiveModule;
	UPROPERTY()
		TArray<int> slotSystemModule;

	float sMaxCompute;
	float sMaxPowerGrid;
	float sMaxCargo;

	float sMaxSpeed;
	float targetSpeed;
	float setedTargetSpeed;
	float currentSpeed;

	float sMaxAcceleration;
	float sMinAcceleration;
	float sStartAccelAngle;
	float accelerationFactor;

	float sMaxRotateRate;
	float sRotateAcceleration;
	float sRotateDeceleration;
	float rotateRateFactor;
	float targetRotateRateFactor;
	float realRotateRateFactor;

	float bonusDroneBaseStats;
	float bonusDroneControl;
	float bonusDroneBay;
	float bonusDroneDamage;
	float bonusDroneRange;
	float bonusDroneSpeed;
#pragma endregion

#pragma region active module temp stats
private:
	float moduleConsumptPower;
	float moduleStatShieldRegen;
	float moduleStatShieldDef;
	float moduleStatArmorRepair;
	float moduleStatArmorDef;
	float moduleStatHullRepair;
	float moduleStatHullDef;
	float moduleStatEngine;
	float moduleStatAcceleration;
	float moduleStatThruster;
#pragma endregion
};