// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "MainTitleMode.h"


AMainTitleMode::AMainTitleMode() {
	
	GameStateClass = ASpaceState::StaticClass();
	PlayerControllerClass = AMainTitleController::StaticClass();
	PlayerStateClass = AUserState::StaticClass();
	DefaultPawnClass = nullptr;

	static ConstructorHelpers::FObjectFinder<UClass> HUDFinder(TEXT("Blueprint'/Game/BluePrint/GameModes/HUD/MainTitleHUD.MainTitleHUD_C'"));
	if(HUDFinder.Succeeded())
		HUDClass = (UClass *)HUDFinder.Object;
}

void AMainTitleMode::BeginPlay() {
	Super::BeginPlay();

}