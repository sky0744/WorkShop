// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "SpaceObject.h"
#include "Interface/Commandable.h"
#include "Ship.generated.h"

/**
 * 
 */
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
	UFUNCTION(BlueprintCallable, Category = "Called For SpaceObject")
		virtual void GetRepaired(GetStatType statType, float repairValue) override;
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
		virtual bool CommandMining(TScriptInterface<ICollectable> target) override;
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

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandToggleTargetModule(int slotIndex, ASpaceObject* target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandToggleActiveModule(int slotIndex) override;
#pragma endregion

#pragma region Functions
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called in Ship")
		BehaviorType GetBehaviorType();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called in Ship")
		ShipClass GetShipClass();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Called in Ship")
		void GetDockedStructure(TScriptInterface<IStructureable>& getStructure);
	
private:
	bool MoveDistanceCheck();
	void RotateCheck();
	void Movement();
	void ModuleCheck();

	bool CheckCanBehavior();
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
public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Ship Data")
		class UBehaviorTree* aiBehaviorTree;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Ship Data")
		UBlackboardComponent* compAIBlackboard;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Ship Data")
		class UBlackboardData* aiBlackboard;
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
	float lengthRader;
	float strategyPoint;
	float bounty;

	UPROPERTY()
		ASpaceObject* targetObject;
	UPROPERTY()
		TScriptInterface<ICommandable> targetCommand;
	UPROPERTY()
		TScriptInterface<IStructureable> targetStructure;
	UPROPERTY()
		TScriptInterface<ICollectable> targetCollect;

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
	UPROPERTY()
		TArray<ASpaceObject*> targetingObject;

	float maxSpeed;
	float targetSpeed;
	float setedTargetSpeed;
	float currentSpeed;

	float maxAcceleration;
	float minAcceleration;
	float startAccelAngle;
	float accelerationFactor;

	float maxRotateRate;
	float rotateAcceleration;
	float rotateDeceleration;
	float rotateRateFactor;
	float targetRotateRateFactor;
	float realRotateRateFactor;

	float bonusCannonLifeTime;
	float bonusRailGunLifeTime;
	float bonusMissileLifeTime;
#pragma endregion
};
