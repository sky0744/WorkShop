// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceDroneController.h"


ASpaceDroneController::ASpaceDroneController() {
#pragma region Controller
	bAttachToPawn = true;
#pragma endregion
#pragma region Replication
	bOnlyRelevantToOwner = false;
	bNetLoadOnClient = false;
#pragma endregion
	compAIBlackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("AIBlackBoard"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> blueprintBehaviorTree(TEXT("BehaviorTree'/Game/BluePrint/AISystem/Drone/DroneBT.DroneBT'"));
	static ConstructorHelpers::FObjectFinder<UBlackboardData> blueprintBlackboard(TEXT("BlackboardData'/Game/BluePrint/AISystem/Drone/DroneBB.DroneBB'"));
	if (blueprintBehaviorTree.Succeeded() && blueprintBlackboard.Succeeded()) {
		aiBehaviorTree = blueprintBehaviorTree.Object;
		aiBlackboard = blueprintBlackboard.Object;
	}
}

void ASpaceDroneController::Possess(APawn* inPawn) {
	Super::Possess(inPawn);
	//Behavior Tree & Blackboard init and Run.
	//if not Inited or invalid pawn, ai never run.
	if (aiBehaviorTree && compAIBlackboard && aiBlackboard) {
		if (inPawn->GetClass() != ADrone::StaticClass())
			return;

		this->InitializeBlackboard(*compAIBlackboard, *aiBlackboard);
		this->RunBehaviorTree(aiBehaviorTree);
		return;

	}
}
