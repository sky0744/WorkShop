// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ProjectNausForBP.h"
#include "MainTitleController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNAUSFORBP_API AMainTitleController : public APlayerController
{
	GENERATED_BODY()
public:
	AMainTitleController();

protected:
	UPROPERTY(VisibleAnyWhere, Category = "Ship Data")
		UCameraComponent* viewpointCamera;
};
