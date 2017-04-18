// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ProjectNausForBP.h"
#include "Enums.h"
#include "SafeENGINE.generated.h"

/**
 * 
 */

#pragma region Sub Data Structure in DataTable Set or Instance
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FNPCTradeData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trade Data")
		int tradeItemID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trade Data")
		int tradeAmountMin;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trade Data")
		int tradeAmountMax;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trade Data")
		int tradeChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trade Data")
		bool isNecessarySupplies;

	FNPCTradeData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FNPCDropData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop Data")
		int dropItemID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop Data")
		int dropAmountMin;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop Data")
		int dropAmountMax;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop Data")
		int dropChance;

	FNPCDropData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FItemSellData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sell Data")
		int sellingItemID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sell Data")
		float sellingChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sell Data")
		int sellingItemMinAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sell Data")
		int sellingItemMaxAmount;

	FItemSellData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FObjectPlacement
{
	GENERATED_USTRUCT_BODY()
public:
	FObjectPlacement() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Respawn Data")
		int objectID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Respawn Data")
		bool isPlacementToGate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Respawn Data")
		int regenChanceFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Respawn Data")
		FVector2D location;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Respawn Data")
		FRotator rotation;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FBonusStat
{
	GENERATED_USTRUCT_BODY()
public:
	FBonusStat(BonusStatType type = BonusStatType::CustomBonus, float stat = 0.0f)
	:	bonusStatType(type)
	,	bonusStat(stat)		{}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bonus State")
		BonusStatType bonusStatType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bonus State")
		float bonusStat;
};
#pragma endregion

#pragma region Usage Data Struct in Actor Instance
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FItem
{
	GENERATED_USTRUCT_BODY()
public:
	FItem(int id = 0, int amount = 0)
		: itemID(id)
		, itemAmount(amount) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Instance Item Data")
		int itemID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Instance Item Data")
		int itemAmount;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FTargetModule
{
	GENERATED_USTRUCT_BODY()
public:
	FTargetModule() 
		: moduleID(0)
		, moduleType(ModuleType::NotModule)
		, isBookedForOff(false)
		, moduleState(ModuleState::NotActivate)
		, maxCooltime(0.0f)
		, remainCooltime(0.0f)

		, maxUsagePower(0.0f)
		, currentUsagePower(0.0f)
		, incrementUsagePower(0.0f)
		, decrementUsagePower(0.0f)

		, damageMultiple(0.0f)
		, launchSpeedMultiple(0.0f)
		, accaucy(0.0f)
		, ammoLifeSpanBonus(0.0f)
		
		, ammo(FItem(0, 0))
		, ammoCapacity(0) {}

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		int moduleID;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		ModuleType moduleType;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		bool isBookedForOff;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		ModuleState moduleState;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float maxCooltime;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float remainCooltime;

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float maxUsagePower;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float currentUsagePower;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float incrementUsagePower;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float decrementUsagePower;

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float damageMultiple;
	//빔계열, 즉시효력형 모듈의 경우 범위.
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float launchSpeedMultiple;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float accaucy;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float ammoLifeSpanBonus;

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		FItem ammo;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		int ammoCapacity;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FActiveModule
{
	GENERATED_USTRUCT_BODY()
public:
	FActiveModule() 
		: moduleID(0)
		, moduleType(ModuleType::NotModule)
		, moduleState(ModuleState::NotActivate)

		, maxUsagePower(0.0f)
		, currentUsagePower(0.0f)
		, incrementUsagePower(0.0f)
		, decrementUsagePower(0.0f)

		, maxActiveModuleFactor(0.0f)
		, currentActiveModuleFactor(0.0f)
		, incrementActiveModuleFactor(0.0f)
		, decrementActiveModuleFactor(0.0f) {}

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		int moduleID;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		ModuleType moduleType;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		ModuleState moduleState;

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float maxUsagePower;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float currentUsagePower;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float incrementUsagePower;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float decrementUsagePower;

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float maxActiveModuleFactor;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float currentActiveModuleFactor;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float incrementActiveModuleFactor;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float decrementActiveModuleFactor;
};

USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSkill
{
	GENERATED_USTRUCT_BODY()
public:
	FSkill(int id = 0, int level = 0, float learning = 0.0f)
		: skillID(id)
		, skillLevel(level)
		, skillLearning(learning) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Instance Skill Data")
		int skillID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Instance Skill Data")
		int canLearnMaxLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Instance Skill Data")
		int skillLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Instance Skill Data")
		float skillLearning;
};
USTRUCT(BlueprintType)		//인덱스 자체가 ID(중복 X)
struct PROJECTNAUSFORBP_API FStructureInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FStructureInfo() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		int structureID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		Faction structureFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		StructureType structureType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		int techLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		FVector2D structureLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		FRotator structureRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		FString LinkedSector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		bool isDestroyed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		bool isRespawnable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float maxRespawnTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float remainRespawnTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float structureShieldRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float structureArmorRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float structureHullRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<FItem> itemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<FItemSellData> itemSellListId;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float maxItemListRefreshTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float remainItemListRefreshTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<FItem> productItemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<int> maxProductAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<FItem> consumptItemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float maxProductTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		float remainProductTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<int> shipSellList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<FItem> playerItemList;
};
#pragma endregion

#pragma region DataTable Set
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FShipData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		FName MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<int> RequireSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<int> RequireSkillLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		Faction Shipfaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		ShipClass Shipclass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float lengthToLongAsix;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float lengthRader;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float Shield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float RechargeShield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float DefShield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float Armor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float RepairArmor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float DefArmor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float Hull;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float RepairHull;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float DefHull;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float Power;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float RechargePower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		int SlotTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		int SlotActive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		int SlotPassive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		int SlotSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float Compute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float PowerGrid;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float Cargo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float MaxSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float MinAcceleration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float MaxAcceleration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float StartAccelAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float MaxRotateRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float rotateAcceleraion;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float rotateDeceleraion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FBonusStat> bonusStats;

	FShipData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FNPCData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		int NPCID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		int ShipID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		FText Desc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		Faction faction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		BehaviorType behaviorType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float strategyPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float npcBounty;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<int> EquipedSlotTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<int> EquipedAmmoForTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FBonusStat> bonusStats;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FNPCTradeData> tradeItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FNPCDropData> dropItems;

	FNPCData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FStationData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		FName MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		Faction faction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		StructureType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float lengthToLongAsix;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float respawnTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float Shield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float RechargeShield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float DefShield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float Armor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float RepairArmor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float DefArmor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float Hull;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float RepairHull;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float DefHull;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<int> ProductItemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<int> ProductAmountList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<int> ConsumItemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<int> ConsumAmountList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<float> ProductTimer;

	FStationData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FResourceData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FName MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		UTexture2D* Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		ResourceType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float lengthToLongAsix;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float respawnTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float Durability;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float DurabilityDef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float ResourceID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float ResourceAmount;

	FResourceData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSkillData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		int SkillID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		int MaxLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		int LearningMultiple;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		TArray<int> RequireSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		TArray<int> RequireSkillLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		BonusStatType BonusType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		float BonusAmountPerLevel;
	FSkillData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		int ItemID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		bool isCanSell;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		bool isCanDrop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		bool isCanReprocess;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		float PointCredit1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		int PointAmount1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		float PointCredit2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		int PointAmount2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		float BuyValueMultiple;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		float CargoVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module In Item Data")
		ItemType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module In Item Data")
		ModuleType ModuleType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module In Item Data")
		ModuleSize ModuleSize;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module In Item Data")
		float UsageCompute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module In Item Data")
		float UsagePowerGrid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module In Item Data")
		TArray<int> RequireSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Module In Item Data")
		TArray<int> RequireSkillLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active or Passive In Item Data")
		BonusStatType StatType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active or Passive In Item Data")
		float BonusStatValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active In Item Data")
		float IncrementActiveModuleFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active In Item Data")
		float DecrementActiveModuleFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active In Item Data")
		float MaxCooltime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active In Item Data")
		float MaxUsagePower;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active In Item Data")
		float IncrementUsagePower;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active In Item Data")
		float DecrementUsagePower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon In Item Data")
		float DamageMultiple;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon In Item Data")
		float LaunchSpeedMultiple;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon In Item Data")
		float Accaucy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon In Item Data")
		float AmmoLifeSpanBonus;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon In Item Data")
		int UsageAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon In Item Data")
		float AmmoCapacity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile In Item Data")
		int ProjectileID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile In Item Data")
		float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile In Item Data")
		float LaunchSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile In Item Data")
		float LifeTime;
	FItemData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FProjectileData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Proejctile Data")
		int ItemID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Proejctile Data")
		int itemLinkedID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Proejctile Data")
		FName MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Proejctile Data")
		UTexture2D* Icon;

	FProjectileData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSectorData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		int SectorID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		FName nSectorName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		FText tSectorName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		Faction SectorFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		SectorType Type;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		SectorState State;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		float SectorDevelopmentLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		float SectorSecurityLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		float SectorTechLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		float SectorEconomyLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		float SectorRecourceLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		TArray<FStructureInfo> StationList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		TArray<FStructureInfo> GateList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		TArray<FObjectPlacement> ShipInitedData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		TArray<FObjectPlacement> ShipRegenData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		int ShipRegenTotal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		TArray<FObjectPlacement> ResourceInitedData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		int ResourceRegenTotal;

	FSectorData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FNewStartPlayerData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		int ProfileID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		int StartShipID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		float StartCredit;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		FString StartSector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		FVector StartPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		TArray<FSkill> StartSkillList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		TArray<FItem> StartItemList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Start Player Data")
		TArray<float> StartFactionRelation;
	FNewStartPlayerData() {}
};
#pragma endregion

UCLASS()
class PROJECTNAUSFORBP_API USafeENGINE : public UGameInstance
{
public:
	GENERATED_BODY()
		USafeENGINE();

#pragma region Get DataTable
	/**
	* Get Ship Data.
	* @param id - The id of the ship.
	* @return Ship Data.
	*/
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FShipData GetShipData(int id);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FNPCData GetNPCData(int id);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FStationData GetStationData(int id);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FResourceData GetResourceData(int id);

	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FSkillData GetSkillData(int id);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FItemData GetItemData(int id);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FProjectileData GetProjectileData(int id);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FSectorData GetSectorData(FString SectorName);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		void GetAllSectorNameData(TArray<FString>& sectorNameArray);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		struct FNewStartPlayerData GetStartProfileData(int id);
#pragma endregion

#pragma region GamePlay - Static
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static int FindItemSlot(UPARAM(ref) TArray<FItem>& itemList, FItem items);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static bool AddCargo(UPARAM(ref) TArray<FItem>& itemList, FItem addItem);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static bool DropCargo(UPARAM(ref) TArray<FItem>& itemList, FItem dropItem);

	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static bool CheckSkill(UPARAM(ref) TArray<FSkill>& skillList, UPARAM(ref) TArray<FSkill>& requsetCheckSkillList);

	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static float CheckDistacneConsiderSize(ASpaceObject* actor1, ASpaceObject* actor2);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static FVector CheckLocationMovetoTarget(ASpaceObject* requestor, ASpaceObject* target, float distance);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static FVector GetRandomLocationToTarget(ASpaceObject* requestor, ASpaceObject* target, float distance);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static FVector GetRandomLocationToLocation(FVector location, float distance);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static FVector GetRandomLocation(bool requestNormalizedVector);
	UFUNCTION(BlueprintCallable, Category = "Call To Manager")
		static FVector GetLocationLimitedSector(FVector location);

#pragma endregion

#pragma region GamePlay - Non Static
public:
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		float CalculateCreditForTrade(int itemID, int lowerAmount, int upperAmount, bool isBuy);
	UFUNCTION(BlueprintCallable, Category = "Call Function : State")
		float CalculateCredit(int itemID, int Amount, bool isBuy);
#pragma endregion

private:
	UPROPERTY()
		UDataTable* ShipData;
	UPROPERTY()
		UDataTable* NPCData;
	UPROPERTY()
		UDataTable* StationData;
	UPROPERTY()
		UDataTable* GateData;
	UPROPERTY()
		UDataTable* ResourceData;

	UPROPERTY()
		UDataTable* SkillData;
	UPROPERTY()
		UDataTable* ItemData;
	UPROPERTY()
		UDataTable* ProjectileData;
	UPROPERTY()
		UDataTable* SectorData;
	UPROPERTY()
		UDataTable* StartPlayerData;
};