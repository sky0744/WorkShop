// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"

#include "SpaceObject.h"
#include "../Interface/Commandable.h"
#include "PlayerShip.generated.h"

#pragma region Constant Value Ship Type - Backup / Real
/*
const float _define_StatDamperMIN = 10.0f;
const float _define_StatDamperMAX = 1000000.0f;
const float _define_StatRestoreMIN = 0.0f;
const float _define_StatRestoreMAX = 500.0f;
const float _define_StatDefMIN = -1000.0f;
const float _define_StatDefMAX = 1000.0f;

const int _define_StatModuleSlotMIN = 0;
const int _define_StatModuleSlotMAX = 8;
*/
const float _define_StatComputePerformanceMIN = 0.0f;
const float _define_StatComputePerformanceMAX = 99999.0f;
const float _define_StatPowerGridPerformanceMIN = 0.0f;
const float _define_StatPowerGridPerformanceMAX = 99999.0f;
const float _define_StatCargoSizeMIN = 0.0f;
const float _define_StatCargoSizeMAX = 200000.0f;
/*
const float _define_StatLengthMIN = 10.0f;
const float _define_StatLengthMAX = 10000.0f;
const float _define_StatRadarRangeMIN = 10.0f;
const float _define_StatRadarRangeMAX = 100000.0f;

const float _define_StatSpeedMIN = 0.0f;
const float _define_StatSpeedMAX = 10000.0f;
const float _define_StatAccelMIN = 0.0f;
const float _define_StatAccelMAX = 1000.0f;
const float _define_StatRotateMIN = 0.0f;
const float _define_StatRotateMAX = 90.0f;

const float _define_StatBonusMIN = 0.0f;
const float _define_StatBonusMAX = 5.0f;
const float _define_StatBonusReducePowerMIN = 0.0f;
const float _define_StatBonusReducePowerMAX = 0.75f;
const float _define_StatBonusReduceCooltimeMIN = 0.0f;
const float _define_StatBonusReduceCooltimeMAX = 0.75f;
const float _define_StatDroneControlMIN = 0.0f;
const float _define_StatDroneControlMAX = 1000.0f;
const float _define_StatDroneBayMIN = 0.0f;
const float _define_StatDroneBayMAX = 5000.0f;

const float _define_DamagedMin = 1.0f;
const float _define_DamagedMAX = 10000.0f;
*/
#pragma endregion

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
		virtual void ControlCamRotateX(const float factorX);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlCamRotateY(const float factorY);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlCamDistance(const float value);

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlViewPointX(const float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlViewPointY(const float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlViewPointOrigin();

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
	void ModuleCheck(const float moduleCheckTime);

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
	UPROPERTY(VisibleAnyWhere, Category = "Ship Data")
		USpringArmComponent* playerViewpointArm;
	UPROPERTY(VisibleAnyWhere, Category = "Ship Data")
		UCameraComponent* playerViewpointCamera;
	UPROPERTY(VisibleAnyWhere, Category = "Ship Data")
		float SmoothZoomRemain;
#pragma endregion

#pragma region Variables
private:
	int sShipID;
	bool sIsInited;

	ShipClass shipClass;
	BehaviorState behaviorState;

	float lengthToLongAsix;
	float lengthRadarRange;

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
		TArray<ASpaceObject*> targetingObject;
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
	float bonusDroneDamage;
	float bonusDroneRange;
	float bonusDroneSpeed;
	float bonusDroneControl;
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