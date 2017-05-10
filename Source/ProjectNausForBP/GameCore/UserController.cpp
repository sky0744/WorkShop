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

#pragma region Location/Camera Controller
	bAttachToPawn = true;

	playerViewpointArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FixArm"));
	playerViewpointCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FixedCamera"));
	playerViewpointArm->SetupAttachment(RootComponent, RootComponent->GetAttachSocketName());
	playerViewpointCamera->SetupAttachment(playerViewpointArm, playerViewpointArm->GetAttachSocketName());

	playerViewpointArm->AddWorldRotation(FRotator(-45.0f, 0.0f, 0.0f));
	playerViewpointArm->bEnableCameraRotationLag = true;
	playerViewpointArm->bEnableCameraLag = true;
	playerViewpointArm->bAbsoluteRotation = true;
	playerViewpointArm->bDoCollisionTest = false;
	playerViewpointArm->bUsePawnControlRotation = false;
	playerViewpointArm->CameraRotationLagSpeed = 7.0f;

	playerCameraMinDistance = _define_CameraDinstanceMIN;
	playerViewpointArm->CameraLagMaxDistance = _define_CameraDinstanceMIN;
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
	if (IsValid(controlledPawn) && controlledPawn->StaticClass()->ImplementsInterface(UCommandable::StaticClass())) {
		commandInterface.SetObject(controlledPawn);
		commandInterface.SetInterface(Cast<ICommandable>(controlledPawn));
	}
	SetAudioListenerOverride(playerViewpointCamera, FVector::ZeroVector, FRotator::ZeroRotator);
	controlledHUD = Cast<ASpaceHUDBase>(GetHUD());

	traceParams = FCollisionQueryParams(FName("PressClick"), true, this);
	mouseRightClicked = false;
}

void AUserController::PlayerTick(float DeltaSeconds) {
	Super::PlayerTick(DeltaSeconds);
}

void AUserController::SetupInputComponent() {
	Super::SetupInputComponent();
	
#pragma region Action Input
	InputComponent->BindAction("OpenInfoProfile", IE_Released, this, &AUserController::OpenInfoProfile);
	InputComponent->BindAction("OpenInfoShip", IE_Released, this, &AUserController::OpenInfoShip);
	InputComponent->BindAction("OpenInfoItem", IE_Released, this, &AUserController::OpenInfoItem);
	InputComponent->BindAction("OpenInfoMap", IE_Released, this, &AUserController::OpenInfoMap);
	InputComponent->BindAction("OpenInfoQuest", IE_Released, this, &AUserController::OpenInfoQuest);
	InputComponent->BindAction("OpenInfoMenu", IE_Released, this, &AUserController::OpenInfoMenu);
	InputComponent->BindAction("UnDock", IE_Released, this, &AUserController::KeyUndock);

	InputComponent->BindAction("ClickLeft", IE_Pressed, this, &AUserController::ClickPressMouseLeft);
	InputComponent->BindAction("ClickLeft", IE_Released, this, &AUserController::ClickReleaseMouseLeft);
	InputComponent->BindAction("ClickWheel", IE_Pressed, this, &AUserController::ClickPressMouseWheel);
	InputComponent->BindAction("ClickWheel", IE_Released, this, &AUserController::ClickReleaseMouseWheel);
	InputComponent->BindAction("ClickRight", IE_Pressed, this, &AUserController::ClickPressMouseRight);
	InputComponent->BindAction("ClickRight", IE_Released, this, &AUserController::ClickReleaseMouseRight);

	InputComponent->BindAction("CamReset", IE_Released, this, &AUserController::ControlCamReset);
#pragma endregion

#pragma region Asix Input
	InputComponent->BindAxis("CamX", this, &AUserController::ControlCamX);
	InputComponent->BindAxis("CamY", this, &AUserController::ControlCamY);

	InputComponent->BindAxis("MouseInX", this, &AUserController::ControlMouseX);
	InputComponent->BindAxis("MouseInY", this, &AUserController::ControlMouseY);
	InputComponent->BindAxis("MouseWheel", this, &AUserController::ControlMouseWheel);
#pragma endregion
}
#pragma endregion

#pragma region Input Binding - Action
void AUserController::ControlCamReset() {
	playerViewpointArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

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

void AUserController::OpenInfoProfile() {
	if (IsValid(controlledHUD))
		controlledHUD->TriggerUI_Profile();
}
void AUserController::OpenInfoShip() {
	if (IsValid(controlledHUD))
		controlledHUD->TriggerUI_Ship();
}
void AUserController::OpenInfoItem() {
	if (IsValid(controlledHUD))
		controlledHUD->TriggerUI_Cargo();
}
void AUserController::OpenInfoMap() {
	if (IsValid(controlledHUD))
		controlledHUD->TriggerUI_Contract();
}
void AUserController::OpenInfoQuest() {
	if (IsValid(controlledHUD))
		controlledHUD->TriggerUI_WorldView();
}
void AUserController::OpenInfoMenu() {
	if (IsValid(controlledHUD))
		controlledHUD->TriggerUI_Menu();
}
void AUserController::KeyUndock() {
	if (IsValid(controlledPawn) && controlledPawn->GetClass()->ImplementsInterface(UCommandable::StaticClass()))
		Cast<ICommandable>(controlledPawn)->CommandUndock();
}
#pragma endregion

#pragma region Input Binding - Asix
void AUserController::ControlCamX(float value) {
	playerViewpointArm->AddWorldOffset(FVector(0.0f, value * _define_CamZoomFactor, 0.0f));
}
void AUserController::ControlCamY(float value) {
	playerViewpointArm->AddWorldOffset(FVector(-value * _define_CamZoomFactor, 0.0f, 0.0f));
}

void AUserController::ControlMouseX(float value) {
	if (!mouseLeftClicked)
		return;
	value *= _define_CameraSensitivityultipleRotate;
	playerViewpointArm->RelativeRotation.Pitch = FMath::Clamp(playerViewpointArm->RelativeRotation.Pitch + value, -75.0f, -10.0f);
	playerViewpointArm->RelativeRotation.Roll = 0.0f;
}
void AUserController::ControlMouseY(float value) {
	if (!mouseLeftClicked)
		return;
	value *= _define_CameraSensitivityultipleRotate;
	playerViewpointArm->RelativeRotation.Yaw -= value;
	playerViewpointArm->RelativeRotation.Pitch = FMath::Clamp(playerViewpointArm->RelativeRotation.Pitch + value, -75.0f, -10.0f);
	playerViewpointArm->RelativeRotation.Roll = 0.0f;
}
void AUserController::ControlMouseWheel(float value) {

	//차후 Tick이나 기타 등등의 방법을 동원해서 Damp Lerp로 목표거리에 도달하도록 할 것. 왜 bEnableCameraLag로는 Lerp가 안되는가...
	value *= _define_CamZoomFactor;
	playerViewpointArm->TargetArmLength = FMath::Clamp(playerViewpointArm->TargetArmLength + value, playerCameraMinDistance, playerViewpointArm->CameraLagMaxDistance);
}

void AUserController::ClickPressMouseLeft(FKey key) {
	mouseLeftClicked = true;
	if(IsValid(controlledHUD))
		controlledHUD->OffUIInteraction();
}
void AUserController::ClickReleaseMouseLeft(FKey key) {
	this->DeprojectMousePositionToWorld(mousePositionInWorld, mouseDirectionInWorld);
	mouseEndPositionInWorld = mousePositionInWorld + mouseDirectionInWorld * 200000.0f;

	mouseLeftClicked = false;
	hitResult.Init();

	this->GetWorld()->LineTraceSingleByObjectType(hitResult
		, mousePositionInWorld
		, mouseEndPositionInWorld
		, traceObjectParams
		, traceParams);

	if (hitResult.bBlockingHit &&hitResult.Actor.Get()->IsA(ASpaceObject::StaticClass())) {
		SetTarget(Cast<ASpaceObject>(hitResult.Actor.Get()));
		UE_LOG(LogClass, Log, TEXT("[Info][UserController][ClickReleaseMouseLeft] Click Object : %s"), *hitResult.Actor.Get()->GetName());
	}
}

void AUserController::ClickPressMouseWheel(FKey key){
}
void AUserController::ClickReleaseMouseWheel(FKey key){
}

void AUserController::ClickPressMouseRight(FKey key) {
	mouseRightClicked = true;
}
void AUserController::ClickReleaseMouseRight(FKey key) {
	
	mouseRightClicked = false;
	this->DeprojectMousePositionToWorld(mousePositionInWorld, mouseDirectionInWorld);
	mouseEndPositionInWorld = mousePositionInWorld + mouseDirectionInWorld * 200000.0f;
	hitResult.Init();

	GetWorld()->LineTraceSingleByObjectType(hitResult
		, mousePositionInWorld
		, mouseEndPositionInWorld
		, traceObjectParams
		, traceParams);

	if (hitResult.bBlockingHit && hitResult.Actor->IsA(ASpaceObject::StaticClass())) 
		SettingInteraction(Cast<ASpaceObject>(hitResult.Actor.Get()));
	else {
		if(IsValid(controlledHUD))
			controlledHUD->OffUIInteraction();
		mouseXYPlane = mousePositionInWorld + mouseDirectionInWorld * FMath::Abs(mousePositionInWorld.Z / mouseDirectionInWorld.Z);	

		if(commandInterface != nullptr && IsValid(commandInterface.GetObjectRef()) && commandInterface.GetInterface() != nullptr)
			commandInterface->CommandMoveToPosition(mouseXYPlane);
	}
}
#pragma endregion

#pragma region player flow control
void AUserController::PlayerShipLengthChange(const int halfLength) {
	playerCameraMinDistance = FMath::Clamp(halfLength * _define_CameraDinstanceMINFactor, _define_CameraDinstanceMIN, _define_CameraDinstanceMAX);
	playerViewpointArm->CameraLagMaxDistance = FMath::Clamp(halfLength * _define_CameraDinstanceMAXFactor, _define_CameraDinstanceMIN, _define_CameraDinstanceMAX);

	playerViewpointArm->CameraLagSpeed = FMath::Abs(playerViewpointArm->CameraLagMaxDistance - playerCameraMinDistance) * FMath::Clamp(1.0f - _define_CameraLerpMultipleZoom, 0.0f, 1.0f);
}

void AUserController::PlayerInterAction(const InteractionType interaction) {

	if (!IsValid(controlledPawn) || (commandInterface != nullptr && IsValid(commandInterface.GetObjectRef()) && commandInterface.GetInterface() != nullptr))
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
		commandInterface->CommandWarp(USafeENGINE::CheckLocationMovetoTarget(controlledPawn, tObj, 500.0f));
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
		if (USafeENGINE::CheckDistanceConsiderSize(controlledPawn, _cargoContainer) > 250.0f)
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
	FColor _peerColor;

	switch (Cast<ASpaceState>(GetWorld()->GetGameState())->PeerIdentify(Faction::Player, tObj->GetFaction(), false)) {
	case Peer::Neutral:
		_peerColor = FColor::Yellow;
		break;
	case Peer::Friendship:
		_peerColor = FColor::Green;
		break;
	case Peer::Enemy:
		_peerColor = FColor::Red;
		break;
	case Peer::TempHold:
		_peerColor = FColor::Blue;
		break;
	default:
		_peerColor = FColor::White;
		break;
	}
	if (IsValid(tObj) && IsValid(controlledHUD))
		controlledHUD->OnUITarget(tObj, _peerColor, 5.0f);
}

void AUserController::SettingInteraction(const ASpaceObject* target) {
	
	FColor peerColor;
	if (!IsValid(target) || controlledPawn == target || controlledPawn == tObj) 
		return;

	if (IsValid(tObj) && IsValid(controlledHUD) && tObj == target)
		controlledHUD->OnUIInteraction(tObj, tObj->GetObjectType());
}
#pragma endregion