// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ProjectNausForBP.h"
#include "Enums.h"
#include "SafeENGINE.generated.h"


#pragma region Constant Value In USafeENGINE
const float _define_LimitSectorSizeMIN = -99000.0f;
const float _define_LimitSectorSizeMAX = 99000.0f;
#pragma endregion

#pragma region Constant Value In SpaceState
const float _define_FactionRelationshipMIN = -10.0f;
const float _define_FactionRelationshipMAX = 10.0f;

const float _define_RelationThresholdAllyStrong = 8.0f;
const float _define_RelationThresholdAlly = 5.0f;
const float _define_RelationThresholdFriend = 2.0f;
const float _define_RelationThresholdBoundary = -2.0f;
const float _define_RelationThresholdEnemy = -5.0f;
const float _define_RelationThresholdEnemyStrong = -8.0f;

const float _define_DamagetoRelationFactorMIN = 0.01f;
const float _define_DamagetoRelationFactorMAX = 0.02f;
const float _define_SPtoRelationFactorMIN = 0.002f;
const float _define_SPtoRelationFactorMAX = 0.005f;
const float _define_LimitApplyRelationPerOnceMIN = -2.0f;
const float _define_LimitApplyRelationPerOnceMAX = 2.0f;

//const float _define_TransAllyRelationship = 0.6f;
//const float _define_TransSubHostileRelationship = -0.15f;
//const float _define_TransHostileRelationship = -0.7f;
#pragma endregion

#pragma region Constant Value UserState Type
const int _define_SkillLevelMIN = 0;
const int _define_SkillLevelMAX = 5;

const float _define_CreditMIN = -999999999999.0f;
const float _define_CreditMAX = 999999999999.0f;

const float _define_RenownMIN = -1000.0f;
const float _define_RenownMAX = 1000.0f;
const float _define_SPToRenownHostile = 0.01f;
const float _define_SPToRenownNotHostile = -0.05f;
#pragma endregion

#pragma region Constant Value In SpaceObject
const float _define_StatDamperMIN = 10.0f;
const float _define_StatDamperMAX = 1000000.0f;
const float _define_StatRestoreMIN = 0.0f;
const float _define_StatRestoreMAX = 500.0f;
const float _define_StatDefMIN = -1000.0f;
const float _define_StatDefMAX = 1000.0f;

const float _define_ModuleANDPathTick = 0.5f;
const int _define_StatModuleSlotMIN = 0;
const int _define_StatModuleSlotMAX = 8;

const float _define_CamDistanceMIN = 100.0f;
const float _define_CamDistanceMAX = 5000.0f;
const float _define_CamZoomFactor = 0.05f;
const float _define_StatLengthMIN = 10.0f;
const float _define_StatLengthMAX = 10000.0f;
const float _define_StatRadarRangeMIN = 10.0f;
const float _define_StatRadarRangeMAX = 100000.0f;
const float _define_StatStrategicPointMIN = 0.0f;
const float _define_StatStrategicPointMAX = 1000.0f;
const float _define_StatBountyMIN = 0.0f;
const float _define_StatBountyMAX = 5000000.0f;

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

const float _define_DamagedMIN = 1.0f;
const float _define_DamagedMAX = 10000.0f;
const float _define_DamagePercentageMIN = 0.15f;
const float _define_DamagePercentageMAX = 4.15f;
#pragma endregion

#pragma region Constant Value Ship Type
const float _define_StatComputePerformanceMIN = 0.0f;
const float _define_StatComputePerformanceMAX = 99999.0f;
const float _define_StatPowerGridPerformanceMIN = 0.0f;
const float _define_StatPowerGridPerformanceMAX = 99999.0f;
const float _define_StatCargoSizeMIN = 0.0f;
const float _define_StatCargoSizeMAX = 200000.0f;

const float _define_SetDistanceToRotateForward = 1000.0f;
#pragma endregion

#pragma region Constant Value Drone Type
#pragma endregion

#pragma region Constant Value Structure Type
const float _define_AvailableDistanceToDock = 300.0f;
const float _define_AvailableDistanceToRestartSet = 500.0f;
const float _define_AvailableDistanceToJump = 500.0f;
#pragma endregion

#pragma region Constant Value Resource Type
const float _define_StatResourceAmountMIN = 0.0f;
const float _define_StatResourceAmountMAX = 10000.0f;
const float _define_RandomRotateSpeedMIN = -3.0f;
const float _define_RandomRotateSpeedMAX = 3.0f;
#pragma endregion

#pragma region Constant Value Cargo Type
const float _define_DropChance_MIN = 0.0f;
const float _define_DropChance_MAX = 100.0f;
#pragma endregion

#pragma region Sub Data Structure in DataTable Set or Instance
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FNPCTradeData
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
struct PROJECTNAUSFORBP_API FNPCDropData
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
		float dropChance;

	FNPCDropData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FItemSellData
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonus State")
		BonusStatType bonusStatType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonus State")
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
struct PROJECTNAUSFORBP_API FHardpoint {
	GENERATED_USTRUCT_BODY()
public:
	FHardpoint() {}
	UPROPERTY(VisibleAnywhere, Category = "HardPoint Data")
		FVector rightLaunchPoint;
	UPROPERTY(VisibleAnywhere, Category = "HardPoint Data")
		FVector leftLaunchPoint;
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
		
		, ammo(FItem(-1, 0))
		, compatibleAmmo(TArray<int>())
		, ammoCapacity(0) 
		, target(nullptr) {}

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		int moduleID;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		ModuleType moduleType;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		ModuleState moduleState;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		FHardpoint hardPoint;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		bool isBookedForOff;

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
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		//빔계열, 지속효력형 모듈의 경우 유효사거리
		float launchSpeedMultiple;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		float accaucy;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		//빔계열, 지속효력형 모듈의 경우 효과의 lifeSpan
		float ammoLifeSpanBonus;

	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		FItem ammo;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		TArray<int> compatibleAmmo;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		int ammoCapacity;
	UPROPERTY(VisibleAnywhere, Category = "Instance Module Data")
		ASpaceObject* target;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instance Skill Data")
		int skillID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instance Skill Data")
		int skillLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instance Skill Data")
		float skillLearning;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FProductProcess {
	GENERATED_USTRUCT_BODY()
public:
	FProductProcess(FItem item = FItem(), float startItem = 0.0f)
		: productItem(item)
		, productTime(startItem)
		, maxProductTime(startItem) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product Data")
		FItem productItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product Data")
		float productTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product Data")
		float maxProductTime;
};
USTRUCT(BlueprintType)		//인덱스 자체가 ID(중복 X)
struct PROJECTNAUSFORBP_API FStructureInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FStructureInfo() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		int structureID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		Faction structureFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		StructureType structureType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		int techLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		FVector2D structureLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		FRotator structureRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		FString LinkedSector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		bool isDestroyed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		bool isRespawnable;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float maxRespawnTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float remainRespawnTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float structureShieldRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float structureArmorRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float structureHullRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		TArray<FItem> itemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<FItemSellData> itemSellListId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float maxItemListRefreshTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float remainItemListRefreshTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<FItem> productItemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<int> maxProductAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<FItem> consumptItemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float maxProductTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float remainProductTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<int> shipSellList;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		TArray<FItem> playerItemList;
};
#pragma endregion

#pragma region DataTable Set
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FShipData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ship Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		FName MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ship Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FSkill> RequireSkills;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		Faction FactionOfProduction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		ShipClass Shipclass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float ShipValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float LengthToLongAsix;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float LengthRadarRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float StrategicPoint;

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
		TArray<FHardpoint> HardPoints;
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
		float RotateAcceleraion;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float RotateDeceleraion;

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
		Faction FactionOfProduction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		BehaviorType BehaviorTypeOfNPC;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float NpcBounty;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		float StrategicPointBonus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<int> EquipedSlotTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<int> EquipedAmmoForTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FBonusStat> BonusStats;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FNPCTradeData> TradeItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Data")
		TArray<FNPCDropData> DropItems;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Structure Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		Faction FactionOfProduction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		StructureType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float LengthToLongAsix;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float StrategicPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Structure Data")
		float RespawnTime;

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
		TArray<FName> MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Resource Data")
		UTexture2D* Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		ResourceType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float LengthToLongAsix;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FVector2D DurabilityRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float DurabilityDef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		int ResourceItemID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FVector2D OreAmountRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float RichOreChance;

	FResourceData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FObjectData : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FText Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FText Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		FName MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Resource Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float LengthToLongAsix;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float Durability;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resource Data")
		float DurabilityDef;
	FObjectData() {}
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FProductData : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product Data")
		FItem ProductItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product Data")
		TArray<FSkill> RequireSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product Data")
		TArray<FItem> RequireItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product Data")
		float ProductTime;

	FProductData() {}
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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skill Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		int SkillID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		SkillGroup SkillGroupType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		int LearningMultiple;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Data")
		TArray<FSkill> RequireSkill;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Data")
		UTexture2D* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		bool isCanSell;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		bool isCanDrop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		bool isCanReprocess;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		FVector2D ValuePointRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		FVector2D AmountPointRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		float BuyValueMultiple;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		float CargoVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
		TArray<FItem> ReprocessResult;

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
		TArray<FSkill> RequireSkill;

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
		TArray<int> UsageAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon In Item Data")
		int AmmoCapacity;

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
		int ItemLinkedID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Proejctile Data")
		FName MeshPath;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Proejctile Data")
		UTexture2D* Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Proejctile Data")
		float ExplosionSensorRange;

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
		Region LocatedRegion;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sector Data")
		FVector2D LocatedInWorldMap;
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
		float StartRenown;
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 함선의 데이터를 획득합니다.
		*	@param id - 획득하고자 하는 함선의 ID입니다.
		*	@return 획득한 함선의 데이터.
		*/
		const struct FShipData& GetShipData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 NPC의 데이터를 획득합니다.
		*	@param id - 획득하고자 하는 NPC의 ID입니다.
		*	@return 획득한 NPC의 데이터.
		*/
		const struct FNPCData& GetNPCData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 스테이션 및 게이트의 데이터를 획득합니다.
		*	FStructureInfo의 데이터와는 별개로, 스텟 및 메쉬 경로 등의 기초 데이터를 포함합니다.
		*	@param id - 획득하고자 하는 구조물의 ID입니다.
		*	@return 획득한 구조물의 데이터.
		*/
		const struct FStationData& GetStationData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 자원 오브젝트의 데이터를 획득합니다.
		*	@param id - 획득하고자 하는 자원 오브젝트의 ID입니다.
		*	@return 획득한 자원 오브젝트의 데이터.
		*/
		const struct FResourceData& GetResourceData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 카고 컨테이너 오브젝트의 데이터를 획득합니다.
		*	- 일반 오브젝트와 동일한 구성이지만, 구분을 위해 별도의 데이터 테이블로 처리합니다.
		*	@param id - 획득하고자 하는 카고 컨테이너 오브젝트의 ID입니다.
		*	@return 획득한 카고 컨테이너 오브젝트의 데이터.
		*/
		const struct FObjectData& GetCargoContainerData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 일반 오브젝트의 데이터를 획득합니다.
		*	이름 및 설명, 메쉬 경로, 인식 크기, 내구도에 대한 단순 데이터만을 필요로 하는 단순 오브젝트에 사용하십시오.
		*	@param id - 획득하고자 하는 오브젝트의 ID입니다.
		*	@return 획득한 오브젝트의 데이터.
		*/
		const struct FObjectData& GetObjectData(const int& id) const;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 스킬의 데이터를 획득합니다.
		*	스킬의 효과 및 선행 조건 등의 데이터를 포함합니다.
		*	@param id - 획득하고자 하는 스킬의 ID입니다.
		*	@return 획득한 스킬의 데이터.
		*/
		const struct FSkillData& GetSkillData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 아이템의 데이터를 획득합니다.
		*	모든 타입의 아이템에 대한 데이터를 포함합니다. 아이템 타입에 무관한 데이터는 Zero-Inited Value
		*	@param id - 획득하고자 하는 오브젝트의 ID입니다.
		*	@return 획득한 오브젝트의 데이터.
		*/
		const struct FItemData& GetItemData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 프로젝타일 오브젝트의 데이터를 획득합니다.
		*	메쉬 경로, 아이템 링크 ID 등에 대한 단순 데이터만을 포함합니다.
		*	@param id - 획득하고자 하는 프로젝타일 오브젝트의 ID입니다.
		*	@return 획득한 프로젝타일 오브젝트의 데이터.
		*/
		const struct FProjectileData& GetProjectileData(const int& id) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 섹터의 데이터를 획득합니다.
		*	모든 섹터의 데이터를 관리하기 위한 시스템에서도 이와 동일한 구조체로 구성되어 있으므로 이에 유의하여 사용하십시오.
		*	New Game에서 섹터 데이터를 셋업하는 용도 및 섹터의 기본 데이터에 접근하는 행위 이 외에는 권장하지 않습니다.
		*	@param id - 획득하고자 하는 섹터의 ID입니다.
		*	@return 획득한 섹터의 데이터.
		*/
		const struct FSectorData& GetSectorData(const FString& SectorName) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 모든 섹터의 이름을 TArray<String>으로 획득합니다.
		*	@param sectorNameArray(out) - 모든 섹터의 이름을 복사할 배열입니다.
		*/
		void GetAllSectorNameData(TArray<FString>& sectorNameArray) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		/**	데이터 테이블로부터 플레이어의 새로운 시작과 관련된 데이터를 획득합니다.
		*	@param id - 획득하고자 하는 플레이어 데이터의 ID입니다.
		*	@return 획득한 플레이어 데이터.
		*/
		const struct FNewStartPlayerData& GetStartProfileData(const int& id) const;
#pragma endregion

#pragma region GamePlay - Static
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static int FindItemSlot(const TArray<FItem>& itemList, const FItem items);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static bool AddCargo(UPARAM(ref) TArray<FItem>& itemList, const FItem addItem);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static bool DropCargo(UPARAM(ref) TArray<FItem>& itemList, const FItem dropItem);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static bool CheckSkill(const TArray<FSkill>& skillList, const TArray<FSkill>& requsetCheckSkillList);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static float CheckDistanceConsiderSize(const ASpaceObject* actor1, const ASpaceObject* actor2);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static FVector CheckLocationMovetoTarget(const ASpaceObject* requestor, const ASpaceObject* target, float distance);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static FVector GetRandomLocationToTarget(const ASpaceObject* requestor, const ASpaceObject* target, float distance);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static FVector GetRandomLocationToLocation(FVector location, float distance);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static FVector GetRandomLocation(bool requestNormalizedVector);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call To Manager")
		static FVector GetLocationLimitedSector(FVector location);

#pragma endregion

#pragma region GamePlay - Non Static
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		float CalculateCreditForTrade(int itemID, int lowerAmount, int upperAmount, bool isBuy) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Call Function : State")
		float CalculateCredit(int itemID, int Amount, bool isBuy) const;
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
		UDataTable* ContainerData;
	UPROPERTY()
		UDataTable* ObjectData;

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