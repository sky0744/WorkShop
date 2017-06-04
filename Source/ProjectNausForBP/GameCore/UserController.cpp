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
	traceParams = FCollisionQueryParams(FName("PressTouch"), true, this);
}

void AUserController::PlayerTick(float DeltaSeconds) {
	Super::PlayerTick(DeltaSeconds);
}

void AUserController::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindTouch(IE_Pressed, this, &AUserController::BeginTouch);
	InputComponent->BindTouch(IE_Repeat, this, &AUserController::RepeatTouch);
	InputComponent->BindTouch(IE_Released, this, &AUserController::EndTouch);

	InputComponent->BindAxis("MouseWheel", this, &AUserController::WheelUp);

	InputComponent->BindAction("MobileBack", IE_Released, this, &AUserController::TouchBack);
	InputComponent->BindAction("MobileMenu", IE_Released, this, &AUserController::TouchMenu);
	InputComponent->BindAction("MouseClick", IE_Released, this, &AUserController::MouseClick);
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

void AUserController::MouseClick(FKey key) {
	FVector _tempVector, _tempDirection;
	FText _text;
	this->DeprojectMousePositionToWorld(_tempVector, _tempDirection);
	_tempVector.Z = 1.0f;

	hitResult.Init();
	GetWorld()->LineTraceSingleByObjectType(hitResult
		, _tempVector
		, _tempVector + _tempDirection * 10000.0f
		, traceObjectParams
		, traceParams);

	if (hitResult.bBlockingHit && hitResult.GetActor()->IsA(ASpaceObject::StaticClass()) && !hitResult.GetActor()->IsA(APlayerShip::StaticClass())) {
		tObj = Cast<ASpaceObject>(hitResult.GetActor());
		
		if (IsValid(controlledHUD)) {
			controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
			controlledHUD->UpdateUI(UpdateUIType::OrderList);
		}
	} else {
		_tempVector.Z = 0.0f;
		commandInterface->CommandMoveToPosition(_tempVector);
		_text = FText::Format(NSLOCTEXT("FTextFieldLiteral", "FTextField", "[클릭-마우스 버전] 위치 : {0}, {1}"), _tempVector.X, _tempVector.Y);

		if (IsValid(controlledHUD)) {
			controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
			controlledHUD->OnUISDestination(_tempVector);
		}
	}
}
void AUserController::WheelUp(float value) {
	FText _text = FText::Format(NSLOCTEXT("FTextFieldLiteral", "FTextField", "[줌-마우스 버전] 줌 배율 : {0}"), value * 0.1f);

	if (value != 0.0f && IsValid(controlledHUD)) {
		controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
		if (controlledHUD->OnMobileDoubleTouch() && IsValid(controlledPawn))
			controlledPawn->ControlCamDistance(value * 0.1f);
	}
}

void AUserController::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {
	FText _text = FText::Format(NSLOCTEXT("FTextFieldLiteral", "FTextField", "[터치 시작] 터치 {0} 위치 : {1}, {2}, {3}"), FingerIndex + 1, Location.X, Location.Y, Location.Z);
	if (IsValid(controlledHUD))
		controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);

	if(FingerIndex == ETouchIndex::Touch1)
		originPositionTouch1 = Location;
	else if (FingerIndex == ETouchIndex::Touch2)
		originPositionTouch2 = Location;
}

void AUserController::RepeatTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {
	FText _text;
	float _zoomMultiple = 1.0f;
	if (FingerIndex == ETouchIndex::Touch2) {
		GetInputTouchState(ETouchIndex::Touch1, originPositionTouch1.X, originPositionTouch1.Y, isTouchingOneIndex);

		if (isTouchingOneIndex) {
			_zoomMultiple = FVector::Dist(currentPositionTouch1, Location) / FVector::Dist(originPositionTouch1, originPositionTouch2);
			_text = FText::Format(NSLOCTEXT("FTextFieldLiteral", "FTextField", "[줌] 줌 배율 : {0}"), _zoomMultiple);
		}
		if (IsValid(controlledHUD)) {
			controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
			if (controlledHUD->OnMobileDoubleTouch() && IsValid(controlledPawn))
				controlledPawn->ControlCamDistance(_zoomMultiple);
		}
	}
}

void AUserController::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location) {
	FText _text;

	if (FingerIndex == ETouchIndex::Touch1) {
		GetInputTouchState(ETouchIndex::Touch1, originPositionTouch1.X, originPositionTouch1.Y, isTouchingOneIndex);

		if (!isTouchingTwoIndex && IsValid(commandInterface.GetObject())) {
			this->DeprojectScreenPositionToWorld(Location.X, Location.Y, touchLocation, touchDirection);

			hitResult.Init();
			GetWorld()->LineTraceSingleByObjectType(hitResult
				, touchLocation
				, touchLocation + touchDirection * 10000.0f
				, traceObjectParams
				, traceParams);

			if (hitResult.bBlockingHit && hitResult.GetActor()->IsA(ASpaceObject::StaticClass()) && !hitResult.GetActor()->IsA(APlayerShip::StaticClass())) {
				tObj = Cast<ASpaceObject>(hitResult.GetActor());
				_text = NSLOCTEXT("FTextFieldLiteral", "FTextField", "[터치 끝] 오브젝트 터치");
				if (IsValid(controlledHUD)) {
					controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
					controlledHUD->UpdateUI(UpdateUIType::OrderList);
				}
			} else {
				touchLocation.Z = 0.0f;
				commandInterface->CommandMoveToPosition(touchLocation);
				_text = FText::Format(NSLOCTEXT("FTextFieldLiteral", "FTextField", "[터치 끝] 이동 위치 : {0}, {1}"), touchLocation.X, touchLocation.Y);

				if (IsValid(controlledHUD)) {
					controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
					controlledHUD->OnUISDestination(touchLocation);
				}
			}
		}
	}
}

void AUserController::TouchBack() {
	FText _text = NSLOCTEXT("FTextFieldLiteral", "FTextField", "[Back] BackButton!");
	if (IsValid(controlledHUD)) {
		controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
		controlledHUD->OnMobileBack();
	}
}

void AUserController::TouchMenu() {
	FText _text = NSLOCTEXT("FTextFieldLiteral", "FTextField", "[Menu] MenuButton!");
	if (IsValid(controlledHUD)) {
		controlledHUD->AddUILogMessage(_text, MessageLogType::Debug, FColor::White);
		controlledHUD->OnMobileMenu();
	}
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
	case InteractionType::UnDock:
		commandInterface->CommandUndock();
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