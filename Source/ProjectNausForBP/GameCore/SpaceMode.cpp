// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceMode.h"


ASpaceMode::ASpaceMode() {

	GameStateClass = ASpaceState::StaticClass();
	PlayerControllerClass = AUserController::StaticClass();
	PlayerStateClass = AUserState::StaticClass();
	DefaultPawnClass = APlayerShip::StaticClass();
	
	static ConstructorHelpers::FObjectFinder<UClass> HUDFinder(TEXT("Blueprint'/Game/BluePrint/HUD/MouseMode/SpaceHUD.SpaceHUD_C''"));
	if (HUDFinder.Succeeded())
		HUDClass = (UClass *)HUDFinder.Object;

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bRunOnAnyThread = false;
	PrimaryActorTick.TickInterval = 20.0f;
}

void ASpaceMode::BeginPlay() {
	Super::BeginPlay();
}

void ASpaceMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
