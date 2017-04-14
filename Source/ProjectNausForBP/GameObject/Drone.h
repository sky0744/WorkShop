// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectNausForBP.h"
#include "SpaceObject.h"
#include "Interface/Commandable.h"
#include "Drone.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNAUSFORBP_API ADrone : public ASpaceObject, public ICommandable
{
	GENERATED_BODY()
#pragma region Event Calls
public:
	ADrone();

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
		virtual bool CommandLaunch(TArray<int> BaySlot) override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandToggleTargetModule(int slotIndex, ASpaceObject* target) override;
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Called by InterFace : Command")
		virtual bool CommandToggleActiveModule(int slotIndex) override;
#pragma endregion

#pragma region Functions

#pragma endregion

#pragma region Components
private:
	UPROPERTY(VisibleAnyWhere, Category = "Drone Data")
		UFloatingPawnMovement* objectMovement;

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Drone Data")
		class UBehaviorTree* aiBehaviorTree;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Drone Data")
		UBlackboardComponent* compAIBlackboard;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Drone Data")
		class UBlackboardData* aiBlackboard;
#pragma endregion

#pragma region Variables
private:
	int droneID;
	Faction faction;
	DroneClass droneClass;
	DroneRuleType droneRuleType;
	BehaviorState behaviorState;

	float lengthToLongAsix;

	FCollisionObjectQueryParams traceObjectParams;
	FHitResult pathFindTraceResult;

	FVector directionFactor;
	FVector moveTargetVector;
	FVector avoidancePathVector;
	bool isNeedToDoAvoidance;

	float maxShield;
	float rechargeShield;
	float currentShield;
	float defShield;

	float maxArmor;
	float currentArmor;
	float defArmor;

	float maxHull;
	float currentHull;
	float defHull;

	//Drone Property 추가 필요...
#pragma endregion
};