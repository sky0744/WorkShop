// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "ProjectNausForBP.h"
#include "SaveLoader.generated.h"

/**
 * 
 */
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
		TArray<FFactionRelationship> relation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FSectorData> sectorInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> npcShipID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FVector> npcShipLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FRotator> npcShipRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<float> npcShipShield;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<float> npcShipArmor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray <float> npcShipHull;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<float> npcShipPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FVector> resourceLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<FRotator> resourceRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> resourceID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<float> resourceDurability;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveLoader)
		TArray<int> resourceAmount;

#pragma endregion

#pragma region Object Data In Current Sector

#pragma endregion
};