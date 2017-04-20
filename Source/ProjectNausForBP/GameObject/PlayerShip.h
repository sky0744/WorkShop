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

#pragma region Funcations : PlayerShip Only
protected:
	bool TotalStatsUpdate();
	void CheckPassiveTypeModule(BonusStatType type, float value);
	void CheckBonusStat(BonusStatType type, float value);
public:
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual bool LoadFromSave(USaveLoader* loader);

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual bool EquipModule(int moduleID);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual bool UnEquipModule(ItemType moduleItemType, int slotNumber);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void GetModule(ItemType moduleType, TArray<int>& moduleList);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void GetModuleActivate(ItemType moduleType, TArray<float>& moduleActivate);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void GetTargetModuleAmmo(TArray<FItem>& targetModuleAmmo);

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual bool SetCameraMode();

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlCamRotateX(float factorX);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlCamRotateY(float factorY);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlCamDistance(float value);

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlViewPointX(float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlViewPointY(float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void ControlViewPointOrigin();

	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void SetTargetSpeed(float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void SetAcceleration(float value);
	UFUNCTION(BlueprintCallable, Category = "Call to Player Ship")
		virtual void SetRotateRate(float value);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool ToggleTargetModule(int slotIndex, ASpaceObject* target);
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool ToggleActiveModule(int slotIndex);
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
		virtual bool CommandLaunch(TArray<int> baySlot) override;
#pragma endregion

#pragma region Functions
private:
	bool MoveDistanceCheck();
	void RotateCheck();
	void Movement();
	void ModuleCheck();

	bool CheckCanBehavior();

	float CalculateCompute();
	float CalculatePowerGrid();
#pragma endregion

#pragma region Path Finder
private:
	void CheckPath();
	void RequestPathUpdate();

	UPROPERTY()
		UNavigationPath* waypointData;
	UPROPERTY()
		TArray<FVector> wayPoint;

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
	float lengthRader;

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
	float targetAccessAngle;
	UPROPERTY()
		TArray<FVector> leftHardPoint;
	UPROPERTY()
		TArray<FVector> rightHardPoint;
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