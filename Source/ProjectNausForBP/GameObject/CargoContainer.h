// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
#include "SpaceObject.h"
#include "CargoContainer.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API ACargoContainer : public ASpaceObject {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACargoContainer();

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
#pragma endregion

#pragma region Functions
	UFUNCTION(BlueprintCallable, Category = "Called For CargoContainer")
		/** 카고 컨테이너의 아이템을 설정
		* @param items - 컨테이너에 추가할 아이템
		*/
		void SetCargo(const FItem& items);
	UFUNCTION(BlueprintCallable, Category = "Called For CargoContainer")
		/** 카고 컨테이너로부터 특정 아이템을 획득 시도
		* @param gettingItem - 획득한 아이템. 반환값에 따라 이를 획득하도록.
		* @return 획득 가능 여부
		*/
		FItem GetCargo();
#pragma endregion

#pragma region Components

#pragma endregion

#pragma region Variables
private:
	int cargoContainerID;
	float containerRotator;
	FItem cargo;
};
