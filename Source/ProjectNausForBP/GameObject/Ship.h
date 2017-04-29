// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "SpaceObject.h"
#include "Interface/Commandable.h"
#include "Ship.generated.h"

#pragma region Constant Value Ship Type - Backup
/*
const float _define_StatDamperMIN = 10.0f;
const float _define_StatDamperMAX = 1000000.0f;
const float _define_StatRestoreMIN = 0.0f;
const float _define_StatRestoreMAX = 500.0f;
const float _define_StatDefMIN = -1000.0f;
const float _define_StatDefMAX = 1000.0f;

const int _define_StatModuleSlotMIN = 0;
const int _define_StatModuleSlotMAX = 8;

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
const float _define_StatDroneControlMAX = 20.0f;

const float _define_DamagedMin = 1.0f;
const float _define_DamagedMAX = 10000.0f;
*/
#pragma endregion

UCLASS()
class PROJECTNAUSFORBP_API AShip : public ASpaceObject, public ICommandable
{
	GENERATED_BODY()
#pragma region Event Calls
public:
	AShip();
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
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to Ship")
		BehaviorType GetBehaviorType() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to Ship")
		ShipClass GetShipClass() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call to Ship")
		void GetDockedStructure(TScriptInterface<IStructureable>& getStructure) const;
	
private:
	bool MoveDistanceCheck();
	void RotateCheck();
	void Movement();
	void ModuleCheck();

	bool CheckCanBehavior() const;
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
	float resultOuter;

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
	int npcShipID;
	bool isInited;

	ShipClass shipClass;
	Faction faction;
	BehaviorType behaviorType;
	BehaviorState behaviorState;

	float lengthToLongAsix;
	float lengthRadarRange;
	float bounty;

	UPROPERTY()
		ASpaceObject* targetObject;
	UPROPERTY()
		TScriptInterface<ICommandable> targetCommand;
	UPROPERTY()
		TScriptInterface<IStructureable> targetStructure;

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

	float maxPower;
	float rechargePower;
	float currentPower;

	UPROPERTY()
		TArray<FTargetModule> slotTargetModule;

	float maxSpeed;
	float targetSpeed;
	float currentSpeed;

	float maxAcceleration;
	float minAcceleration;
	float startAccelAngle;
	float accelerationFactor;

	float maxRotateRate;
	float rotateAcceleration;
	float rotateDeceleration;
	float targetRotateRateFactor;
	float realRotateRateFactor;

	float bonusCannonLifeTime;
	float bonusRailGunLifeTime;
	float bonusMissileLifeTime;

	float moduleConsumptPower;
#pragma endregion
};
