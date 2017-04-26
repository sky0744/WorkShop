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
	ControlledPawn = Cast<APlayerShip>(GetPawn());
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


#pragma region Input Binding
#pragma region Input Binding - Action
void AUserController::ControlCamReset() {
	//GEngine->AddOnScreenDebugMessage(-1, 0.0333f, FColor::Yellow, "Control Roll : " + FString::SanitizeFloat(value));
	ControlledPawn->ControlViewPointOrigin();
}

void AUserController::ControlTargetSpeed(float value) {
	ControlledPawn->SetTargetSpeed(value);
}
void AUserController::ControlAcceleration(float value) {
	ControlledPawn->SetAcceleration(value);
}
void AUserController::ControlRotateSpeed(float value) {
	ControlledPawn->SetRotateRate(value);
}

void AUserController::OpenInfoProfile() {

}
void AUserController::OpenInfoShip() {

}
void AUserController::OpenInfoItem() {

}
void AUserController::OpenInfoMap() {

}
void AUserController::OpenInfoQuest() {

}
void AUserController::OpenInfoMenu() {

}
void AUserController::KeyUndock() {
	Cast<ICommandable>(ControlledPawn)->CommandUndock();
}
#pragma endregion

#pragma region Input Binding - Asix
void AUserController::ControlCamX(float value) {
	ControlledPawn->ControlViewPointX(value * 20.0f/*AUserState.ev_KeyAsixSensitivity*/);
}
void AUserController::ControlCamY(float value) {
	ControlledPawn->ControlViewPointY(value * 20.0f/*AUserState.ev_KeyAsixSensitivity*/);
}

void AUserController::ControlMouseX(float value) {
	if (mouseLeftClicked == true) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.0333f, FColor::Yellow, "Control Cam X : " + FRotator(0.0f, value, 0.0f).ToString());
		ControlledPawn->ControlCamRotateX(value);
	}
}
void AUserController::ControlMouseY(float value) {
	if (mouseLeftClicked == true) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.0333f, FColor::Yellow, "Control Cam Y : " + FRotator(-value, 0.0f, 0.0f).ToString());
		ControlledPawn->ControlCamRotateY(value);
	}
}
void AUserController::ControlMouseWheel(float value) {
	//GEngine->AddOnScreenDebugMessage(-1, 0.0333f, FColor::Yellow, "Control Zoom : " + FString::SanitizeFloat(value));
	ControlledPawn->ControlCamDistance(value * 10.0f);
}

void AUserController::ClickPressMouseLeft(FKey key) {
	mouseLeftClicked = true;
	Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OffUIInteraction();
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
	this->DeprojectMousePositionToWorld(mousePositionInWorld, mouseDirectionInWorld);
	mouseEndPositionInWorld = mousePositionInWorld + mouseDirectionInWorld * 200000.0f;
	hitResult.Init();

	this->GetWorld()->LineTraceSingleByObjectType(hitResult
		, mousePositionInWorld
		, mouseEndPositionInWorld
		, traceObjectParams
		, traceParams);

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
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OffUIInteraction();
		mouseXYPlane = mousePositionInWorld + mouseDirectionInWorld * FMath::Abs(mousePositionInWorld.Z / mouseDirectionInWorld.Z);	
		mouseXYPlane.Z = 0.0f;
		ICommandable* _possessPawn = Cast<ICommandable>(ControlledPawn);

		FNavLocation _tempNavLoc = FNavLocation();
		if(UNavigationSystem::GetCurrent(GetWorld())->ProjectPointToNavigation(mouseXYPlane, _tempNavLoc))
			_possessPawn->CommandMoveToPosition(mouseXYPlane);
	}
}
#pragma endregion

#pragma region player flow control
void AUserController::PlayerInterAction(InteractionType interaction) {

	ICommandable* _pObj;
	if (!ControlledPawn->GetClass()->ImplementsInterface(UCommandable::StaticClass()))
		return;
	_pObj = Cast<ICommandable>(ControlledPawn);

	TScriptInterface<IStructureable> _sObj;
	AResource* _resource;

	_pObj->CommandStop();

	switch (interaction)
	{
	case InteractionType::DockRequest:
		if (!tObj->GetClass()->ImplementsInterface(UStructureable::StaticClass()))
			return;

		_sObj.SetObject(tObj);
		_sObj.SetInterface(Cast<IStructureable>(tObj));
		_pObj->CommandDock(_sObj);
		break;
	case InteractionType::Approach:
		_pObj->CommandMoveToTarget(tObj);
		break;
	case InteractionType::Attack:
		_pObj->CommandAttack(tObj);
		break;
	case InteractionType::Jump:
		if (!tObj->GetClass()->ImplementsInterface(UStructureable::StaticClass()))
			return;

		_sObj.SetObject(tObj);
		_sObj.SetInterface(Cast<IStructureable>(tObj));
		_pObj->CommandJump(_sObj);
		break;
	case InteractionType::Warp:
		_pObj->CommandWarp( USafeENGINE::CheckLocationMovetoTarget(ControlledPawn, tObj, 500.0f) );
		break;
	case InteractionType::Collect:
		if (!tObj->IsA(AResource::StaticClass()))
			return;
		_resource = Cast<AResource>(tObj);
		_pObj->CommandMining(_resource);
		break;
	case InteractionType::Repair:
		_pObj->CommandRepair(tObj);
		break;
	default:
		break;
	}
}

bool AUserController::SetWarpLocation(FVector location) {
	//if location is invaild / too far(out of normal zone), cancel setting warp location.
	if (FVector::Dist(FVector::ZeroVector, location) > 500000.0f)
		return false;

	warpLocation = location;
	return true;
}

bool AUserController::ToggleTargetModule(int slotIndex) {
	return ControlledPawn->ToggleTargetModule(slotIndex, tObj);
}

bool AUserController::ToggleActiveModule(int slotIndex) {
	return ControlledPawn->ToggleActiveModule(slotIndex);
}

ASpaceObject* AUserController::GetTargetInfo() {
	if(USafeENGINE::IsValid(tObj) && tObj != ControlledPawn)
		return tObj;
	else return nullptr;
}

void AUserController::SetTarget(ASpaceObject* target) {

	if (target->IsA(ASpaceObject::StaticClass())) {

		FColor _peerColor;
		if (ControlledPawn == target)
			return;

		if (!USafeENGINE::IsValid(tObj) || tObj != target) {
			tObj = target;
			switch (Cast<ASpaceState>(GetWorld()->GetGameState())->PeerIdentify(Faction::Player, tObj->GetFaction())) {
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
			Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OnUITarget(tObj, _peerColor, 5.0f);
		} else if (tObj == target)
			tObj = nullptr;
		else
			return;
	}
}

void AUserController::SettingInteraction(ASpaceObject* target) {
	
	FColor peerColor;
	if (ControlledPawn == target)
		return;
	if (tObj == target)
		Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OnUIInteraction(tObj, tObj->GetObjectType());
}
#pragma endregion