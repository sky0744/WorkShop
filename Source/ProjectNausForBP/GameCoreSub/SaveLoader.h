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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		int npcShipID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		Faction npcShipFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FVector2D npcShipLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FRotator npcShipRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float npcShipShield;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float npcShipArmor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float npcShipHull;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		int droneID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		Faction droneFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FVector2D droneLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FRotator droneRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float droneShield;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float droneArmor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		int spaceObjectID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		Faction spaceObjectFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FVector2D spaceObjectLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FRotator spaceObjectRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float spaceObjectDurability;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSavedContainerData {
	GENERATED_USTRUCT_BODY()
public:
	FSavedContainerData(int id = 0, FVector2D location = FVector2D(), FRotator rotation = FRotator(),
		float durability = .0f, TArray<FItem> cargo = TArray<FItem>())
		: containerID(id)
		, containerLocation(location)
		, containerRotation(rotation)
		, containerDurability(durability)
		, containerCargo(TArray<FItem>(cargo)) {
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		int containerID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FVector2D containerLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FRotator containerRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float containerDurability;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FItem> containerCargo;
};
USTRUCT(BlueprintType)
struct PROJECTNAUSFORBP_API FSavedResourceData {
	GENERATED_USTRUCT_BODY()
public:
	FSavedResourceData(int id = 0, FVector2D location = FVector2D(), FRotator rotation = FRotator(),
		float durability = .0f, int resourceAmount = 0)
		: resourceID(id)
		, resourceLocation(location)
		, resourceRotation(rotation)
		, resourceDurability(durability)
		, resourceAmount(resourceAmount) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		int resourceID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FVector2D resourceLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FRotator resourceRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float resourceDurability;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		int resourceAmount;
};

UCLASS()
class PROJECTNAUSFORBP_API USaveLoader : public USaveGame
{
	GENERATED_BODY()
		USaveLoader();
public:

#pragma region Player Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FText name;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		int shipID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FString sectorName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FText playerFactionName;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float credit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		SaveState saveState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FVector position;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		FRotator rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float hull;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float armor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float shield;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		float power;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> slotTargetModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> slotActiveModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> slotPassiveModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> slotSystemModule;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FItem> targetModuleAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSkill> skillList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FItem> itemList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FItem> playerDronesInHanger;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSavedDroneData> playerDronesInSpace;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> skillIDList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> skillLevelList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<float> skillLearnList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> itemIDList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> itemAmountList;

#pragma endregion

#pragma region Sector Data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<float> relation;
	//sector Data -> Structure Data Include.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSectorData> sectorInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSavedNPCShipData> shipSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSavedDroneData> droneSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSavedObjectData> objectSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSavedContainerData> containerSaveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSavedResourceData> resourceSaveInfo;
#pragma endregion

#pragma region Object Data In Current Sector

#pragma endregion
};