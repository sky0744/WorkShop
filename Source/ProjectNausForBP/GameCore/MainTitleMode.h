// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "ProjectNausForBP.h"
#include "MainTitleMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNAUSFORBP_API AMainTitleMode : public AGameMode
{
	GENERATED_BODY()
public:
	AMainTitleMode();
#pragma region Base GameMode
protected:
	virtual void BeginPlay() override;
#pragma endregion
	
};
