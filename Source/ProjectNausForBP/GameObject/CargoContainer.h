// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpaceObject.h"
#include "CargoContainer.generated.h"

#define _def_DropChance_MIN 0.0f
#define _def_DropChance_MAX 1.0f

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
#pragma endregion

#pragma region Functions
	/** 카고 컨테이너의 아이템을 설정
	* @param objectType - 함수를 호출한 오브젝트의 타입
	* @param ObjectIDforDrop - 함수를 호출한 오브젝트의 ID
	*/
	void SetCargo(ObjectType objectType, int ObjectIDforDrop);
	/** 카고 컨테이너의 아이템을 설정
	* @param items - 컨테이너에 추가할 아이템
	* @param dropChance - 개별 아이템의 드롭 찬스.
	* 참고 : 범위는 0.0f(0%) ~ 1.0f(100%)로 일괄 적용.
	*/
	void SetCargo(UPARAM(ref) TArray<FItem>& items, float dropChance);

	UFUNCTION(BlueprintCallable, Category = "Called For CargoContainer")
		/** 카고 컨테이너의 아이템 추가
		* @param items - 컨테이너에 추가할 아이템
		*/
		void AddCargo(UPARAM(ref) TArray<FItem>& items);

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
public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Container Data")
		TArray<FItem> cargo;
};
