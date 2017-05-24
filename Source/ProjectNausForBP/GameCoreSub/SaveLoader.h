// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "ProjectNausForBP.h"
#include "SaveLoader.generated.h"

USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSavedNPCShipData {
	GENERATED_USTRUCT_BODY()
public:
	FSavedNPCShipData(int id = 0, Faction faction = Faction::Neutral, FVector2D location = FVector2D(), FRotator rotation = FRotator(),
		float shield = .0f, float armor = .0f, float hull = .0f, float power = .0f )
		: npcShipID(id)
		, npcShipFaction(faction)
		, npcShipLocation(location)
		, npcShipRotation(rotation)
		, npcShipShield(shield)
		, npcShipArmor(armor)
		, npcShipHull(hull)
		, npcShipPower(power) {}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int npcShipID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		Faction npcShipFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FVector2D npcShipLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FRotator npcShipRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float npcShipShield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float npcShipArmor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float npcShipHull;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float npcShipPower;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSavedDroneData {
	GENERATED_USTRUCT_BODY()
public:
	FSavedDroneData(int id = 0, Faction faction = Faction::Neutral, FVector2D location = FVector2D(),
		FRotator rotation = FRotator(), float shield = .0f, float armor = .0f, float hull = .0f)
		: droneID(id)
		, droneFaction(faction)
		, droneLocation(location)
		, droneRotation(rotation)
		, droneShield(shield)
		, droneArmor(armor)
		, droneHull(hull) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int droneID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		Faction droneFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FVector2D droneLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FRotator droneRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float droneShield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float droneArmor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float droneHull;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSavedObjectData {
	GENERATED_USTRUCT_BODY()
public:
	FSavedObjectData(int id = 0, Faction faction = Faction::Neutral, FVector2D location = FVector2D(),
		FRotator rotation = FRotator(), float durability = .0f)
		: spaceObjectID(id)
		, spaceObjectFaction(faction)
		, spaceObjectLocation(location)
		, spaceObjectRotation(rotation)
		, spaceObjectDurability(durability) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int spaceObjectID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		Faction spaceObjectFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FVector2D spaceObjectLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FRotator spaceObjectRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float spaceObjectDurability;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSavedContainerData {
	GENERATED_USTRUCT_BODY()
public:
	FSavedContainerData(int id = 0, FVector2D location = FVector2D(), FRotator rotation = FRotator(),
		float durability = .0f, FItem cargo = FItem())
		: containerID(id)
		, containerLocation(location)
		, containerRotation(rotation)
		, containerDurability(durability)
		, containerCargo(FItem(cargo)) {
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int containerID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FVector2D containerLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FRotator containerRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float containerDurability;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FItem containerCargo;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSavedResourceData {
	GENERATED_USTRUCT_BODY()
public:
	FSavedResourceData(int id = 0, FVector2D location = FVector2D(), FRotator rotation = FRotator(), float durability = 0.0f, int resourceCount = 0)
		: resourceID(id)
		, resourceLocation(location)
		, resourceRotation(rotation)
		, resourceDurability(durability)
		, resourceAmount(resourceCount) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int resourceID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FVector2D resourceLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FRotator resourceRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float resourceDurability;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int resourceAmount;
};

UCLASS()
class PROJECTNAUSFORBP_API USaveLoader : public USaveGame
{
	GENERATED_BODY()
		USaveLoader();
public:

#pragma region Player Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FText name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int shipID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FString sectorName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FString restartSector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FVector2D restartLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FText playerFactionName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float relationwithPlayerEmpire;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float credit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		SaveState saveState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FVector2D position;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		FRotator rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float hull;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float armor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float shield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		float power;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<int> slotTargetModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<int> slotActiveModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<int> slotPassiveModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<int> slotSystemModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FItem> targetModuleAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSkill> userSkillData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		int userLearningSkillId;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FItem> userItemData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FItem> playerDronesInHanger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSavedDroneData> playerDronesInSpace;
#pragma endregion

#pragma region Sector Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<float> relation;
	//sector Data -> Structure Data Include.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSectorData> sectorInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSavedNPCShipData> shipSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSavedDroneData> droneSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSavedObjectData> objectSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSavedContainerData> containerSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SaveLoader)
		TArray<FSavedResourceData> resourceSaveInfo;
#pragma endregion

#pragma region Object Data In Current Sector

#pragma endregion
};