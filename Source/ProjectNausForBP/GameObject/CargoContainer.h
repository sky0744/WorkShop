// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpaceObject.h"
#include "CargoContainer.generated.h"

#pragma region Constant Value Ship Type - Backup / Real
/*
const float _define_StatDamperMIN = 10.0f;
const float _define_StatDamperMAX = 1000000.0f;
const float _define_StatDefMIN = -1000.0f;
const float _define_StatDefMAX = 1000.0f;

const float _define_DamagedMin = 1.0f;
const float _define_DamagedMAX = 10000.0f;
*/
const float _define_DropChance_MIN = 0.0f;
const float _define_DropChance_MAX = 100.0f;

#pragma endregion


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
		* @param objectType - 함수를 호출한 오브젝트의 타입
		* @param ObjectIDforDrop - 함수를 호출한 오브젝트의 ID
		*/
		void SetCargoFromData(const ObjectType objectType, const int ObjectIDforDrop);
	UFUNCTION(BlueprintCallable, Category = "Called For CargoContainer")
		/** 카고 컨테이너의 아이템을 설정
		* @param items - 컨테이너에 추가할 아이템
		* @param dropChance - 개별 아이템의 드롭 찬스.
		* 참고 : 범위는 0.0f(0%) ~ 100.0f(100%)로 일괄 적용.
		*/
		void SetCargo(const TArray<FItem>& items, float dropChance);

	UFUNCTION(BlueprintCallable, Category = "Called For CargoContainer")
		/** 카고 컨테이너의 아이템 추가
		* @param items - 컨테이너에 추가할 아이템
		*/
		void AddCargo(const TArray<FItem>& items);

	UFUNCTION(BlueprintCallable, Category = "Called For CargoContainer")
		/** 카고 컨테이너로부터 모든 아이템을 획득 시도
		* @param GettingItems - 획득에 성공한 아이템
		*/
		void GetAllCargo(TArray<FItem>& gettingItems);

	UFUNCTION(BlueprintCallable, Category = "Called For CargoContainer")
		/** 카고 컨테이너로부터 특정 아이템을 획득 시도
		* @param cargoSlot - 획득을 시도할 아이템 슬롯
		* @param gettingItem - 획득한 아이템. 반환값에 따라 이를 획득하도록.
		* @return 획득 가능(유효 슬롯) 여부
		*/
		bool GetCargo(int cargoSlot, FItem& gettingItem);
#pragma endregion

#pragma region Components

#pragma endregion

#pragma region Variables
private:
	int cargoContainerID;
	FRotator containerRotator;
public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Container Data")
		TArray<FItem> cargo;
};
