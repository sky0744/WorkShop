// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "SpaceAIController.h"

ASpaceAIController::ASpaceAIController() {
#pragma region Controller
	bAttachToPawn = true;
#pragma endregion
#pragma region Replication
	bOnlyRelevantToOwner = false;
	bNetLoadOnClient = false;
#pragma endregion
	compAIBlackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("AIBlackBoard"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> blueprintBehaviorTree(TEXT("BehaviorTree'/Game/BluePrint/AISystem/Ship/SpaceAIBT.SpaceAIBT'"));
	static ConstructorHelpers::FObjectFinder<UBlackboardData> blueprintBlackboard(TEXT("BlackboardData'/Game/BluePrint/AISystem/Ship/SpaceAIBB.SpaceAIBB'"));
	if (blueprintBehaviorTree.Succeeded() && blueprintBlackboard.Succeeded()) {
		aiBehaviorTree = blueprintBehaviorTree.Object;
		aiBlackboard = blueprintBlackboard.Object;
	}
}

void ASpaceAIController::Possess(APawn* inPawn) {
	Super::Possess(inPawn);
	//Behavior Tree & Blackboard init and Run.
	//if not Inited or invalid pawn, ai never run.
	if (IsValid(inPawn) && aiBehaviorTree && compAIBlackboard && aiBlackboard) {
		if (inPawn->GetClass() != AShip::StaticClass())
			return;

		this->InitializeBlackboard(*compAIBlackboard, *aiBlackboard);
		this->RunBehaviorTree(aiBehaviorTree);
		return;
	}
}
