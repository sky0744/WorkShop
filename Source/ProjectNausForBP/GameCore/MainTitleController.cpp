// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "MainTitleController.h"

AMainTitleController::AMainTitleController() {

#pragma region Player Controller
	bAutoManageActiveCameraTarget = false;
	PlayerCameraManagerClass = nullptr;
	SmoothTargetViewRotationSpeed = 20.0f;
	InputYawScale = 2.0f;
	InputPitchScale = -2.0f;
	InputRollScale = 2.0f;
#pragma endregion

#pragma region Mouse Interface
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = true;
	bEnableTouchOverEvents = true;

	DefaultClickTraceChannel = ECollisionChannel::ECC_Visibility;
	HitResultTraceDistance = 2000000.0f;
#pragma endregion

#pragma region Controller
	bAttachToPawn = false;
#pragma endregion

#pragma region Replication
	bOnlyRelevantToOwner = false;
	bNetLoadOnClient = false;
#pragma endregion

	viewpointCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FixedCamera"));
	viewpointCamera->SetupAttachment(RootComponent, RootComponent->GetAttachSocketName());
	viewpointCamera->bAbsoluteRotation = true;
	//playerViewpointCamera->AddWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	viewpointCamera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	viewpointCamera->OrthoWidth = 1024.0f;
}
