// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "UserController.h"

AUserController::AUserController() {
#pragma region Player Controller
	bAutoManageActiveCameraTarget = true;
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
	bAttachToPawn = true;
#pragma endregion

#pragma region Replication
	bOnlyRelevantToOwner = false;
	bNetLoadOnClient = false;
#pragma endregion
}

#pragma region Event Calls
void AUserController::BeginPlay() {
	Super::BeginPlay();
	controlledPawn = Cast<APlayerShip>(GetPawn());
	if (IsValid(controlledPawn)) {
		SetAudioListenerOverride(controlledPawn->GetCamera(), FVector::ZeroVector, FRotator::ZeroRotator);
		if (controlledPawn->StaticClass()->ImplementsInterface(UCommandable::StaticClass())) {
			commandInterface.SetObject(controlledPawn);
			commandInterface.SetInterface(Cast<ICommandable>(controlledPawn));
		}
	}
	controlledHUD = Cast<ASpaceHUDBase>(GetHUD());

	traceParams = FCollisionQueryParams(FName("PressClick"), true, this);
	isMultiTouching = false;
}

void AUserController::PlayerTick(float DeltaSeconds) {
	Super::PlayerTick(DeltaSeconds);
}

void AUserController::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindTouch(IE_Pressed, this, &AUserController::BeginTouch);
	InputComponent->BindTouch(IE_Repeat, this, &AUserController::RepeatTouch);
	InputComponent->BindTouch(IE_Released, this, &AUserController::EndTouch);

	InputComponent->BindAction("MobileBack", IE_Released, this, &AUserController::TouchBack);
	InputComponent->BindAction("MobileMenu", IE_Released, this, &AUserController::TouchMenu);
}
#pragma endregion

#pragma region Input Binding
void AUserController::ControlTargetSpeed(float value) {
	if(IsValid(controlledPawn))
		controlledPawn->SetTargetSpeed(value);
}
void AUserController::ControlAcceleration(float value) {
	if (IsValid(controlledPawn))
		controlledPawn->SetAcceleration(value);
}
void AUserController::ControlRotateSpeed(float value) {
	if (IsValid(controlledPawn))
		controlledPawn->SetRotateRate(value);
}

void AUserController::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {
	GEngine->AddOnScreenDebugMessage(10, 2.0f, FColor::White, "[Begin] Touch " + FString::FromInt(FingerIndex) + " Point pos : " + Location.ToString());
}

void AUserController::RepeatTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {
	GEngine->AddOnScreenDebugMessage(10, 2.0f, FColor::White, "[Repeat] Touch " + FString::FromInt(FingerIndex) + " Point pos : " + Location.ToString());
}

void AUserController::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {
	GEngine->AddOnScreenDebugMessage(10, 2.0f, FColor::White, "[End] Touch " + FString::FromInt(FingerIndex) + " Point pos : " + Location.ToString());
}

void AUserController::TouchBack() {
	controlledHUD->OnMobileBack();
}

void AUserController::TouchMenu() {
	controlledHUD->OnMobileMenu();
}
#pragma endregion

#pragma region player flow control
void AUserController::PlayerInterAction(const InteractionType interaction) {

	if (!IsValid(controlledPawn) || commandInterface == nullptr)
		return;

	TScriptInterface<IStructureable> _sObj;
	AResource* _resource;
	ACargoContainer* _cargoContainer;
	FItem _item;

	commandInterface->CommandStop();
	switch (interaction)
	{
	case InteractionType::DockRequest:
		if (!tObj->GetClass()->ImplementsInterface(UStructureable::StaticClass()))
			return;

		_sObj.SetObject(tObj);
		_sObj.SetInterface(Cast<IStructureable>(tObj));
		commandInterface->CommandDock(_sObj);
		break;
	case InteractionType::Approach:
		commandInterface->CommandMoveToTarget(tObj);
		break;
	case InteractionType::Attack:
		commandInterface->CommandAttack(tObj);
		break;
	case InteractionType::Jump:
		if (!tObj->GetClass()->ImplementsInterface(UStructureable::StaticClass()))
			return;

		_sObj.SetObject(tObj);
		_sObj.SetInterface(Cast<IStructureable>(tObj));
		commandInterface->CommandJump(_sObj);
		break;
	case InteractionType::Warp:
		commandInterface->CommandWarp(tObj->GetActorLocation());
		break;
	case InteractionType::Collect:
		if (!tObj->IsA(AResource::StaticClass()))
			return;
		_resource = Cast<AResource>(tObj);
		commandInterface->CommandMining(_resource);
		break;
	case InteractionType::Repair:
		commandInterface->CommandRepair(tObj);
		break;
	case InteractionType::GetCargo:
		if (!tObj->IsA(ACargoContainer::StaticClass()))
			return;
		_cargoContainer = Cast<ACargoContainer>(tObj);
		if (controlledPawn->GetDistanceTo(_cargoContainer) > _define_AvailableDistanceGetCargo)
			return;
		_item = _cargoContainer->GetCargo();
		if (Cast<AUserState>(PlayerState)->AddPlayerCargo(_item))
			_cargoContainer->Destroy();
		else
			_cargoContainer->SetCargo(_item);
		break;
	default:
		break;
	}
}

bool AUserController::ToggleTargetModule(const int slotIndex) {
	if (IsValid(controlledPawn))
		return controlledPawn->ToggleTargetModule(slotIndex, tObj);
	else return false;
}

bool AUserController::ToggleActiveModule(const int slotIndex) {
	if (IsValid(controlledPawn))
		return controlledPawn->ToggleActiveModule(slotIndex);
	else return false;
}

ASpaceObject* AUserController::GetTargetInfo() {

	if(IsValid(tObj) && tObj != controlledPawn)
		return tObj;
	else {
		tObj = nullptr;
		return nullptr;
	}
}

void AUserController::SetTarget(ASpaceObject* target) {

	if (!IsValid(target) || controlledPawn == target || tObj == target) {
		tObj = nullptr;
		return;
	}
	else
		tObj = target;
}
#pragma endregion