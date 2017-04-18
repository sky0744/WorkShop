// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "ProjectNausForBP.h"
#include "SpaceMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNAUSFORBP_API ASpaceMode : public AGameMode {
	GENERATED_BODY()
public:
	ASpaceMode();

#pragma region Base GameMode
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
#pragma endregion
};
