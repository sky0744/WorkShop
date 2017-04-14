// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Station.h"

AStation::AStation() {
	objectMesh->SetCanEverAffectNavigation(true);
	objectMesh->SetEnableGravity(false);
	objectMesh->SetSimulatePhysics(true);
	objectMesh->BodyInstance.MassScale = 100.0f;
	objectMesh->BodyInstance.LinearDamping = 50.0f;
	objectMesh->BodyInstance.AngularDamping = 5000.0f;
	objectMesh->BodyInstance.bLockZTranslation = true;
	objectMesh->BodyInstance.bLockXRotation = true;
	objectMesh->BodyInstance.bLockYRotation = true;
	objectMesh->Mobility = EComponentMobility::Movable;
	RootComponent = objectMesh;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 5.0f;
}

#pragma region Event Call
void AStation::BeginPlay() {
	Super::BeginPlay();

	UE_LOG(LogClass, Log, TEXT("[Info][Station][Spawn] Spawn Finish!"));
}

void AStation::Tick(float DeltaSeconds) {

	currentShield = FMath::Clamp(currentShield + rechargeShield * 5.0f, 0.0f, maxShield);
	currentArmor = FMath::Clamp(currentArmor + repairArmor * 5.0f, 0.0f, maxArmor);
	currentHull = FMath::Clamp(currentHull + repairHull * 5.0f, 0.0f, maxHull);

	if (structureInfo->remainItemListRefreshTime >= 0.0f)
		structureInfo->remainItemListRefreshTime = FMath::Clamp(structureInfo->remainItemListRefreshTime - DeltaSeconds, 0.0f, structureInfo->maxItemListRefreshTime);
	if (structureInfo->remainItemListRefreshTime <= 0.0f && structureInfo->structureType != StructureType::ProductionFacility) {
		int findIndex = -1;
		int addAmount = 0;
		FItem item = FItem();

		for (int index = 0; index < structureInfo->itemSellListId.Num(); index++) {
			if (structureInfo->itemSellListId[index].sellingChance >= FMath::RandRange(0.0, 100.0f)) {
				item = FItem(structureInfo->itemSellListId[index].sellingItemID, 1);
				findIndex = USafeENGINE::FindItemSlot(structureInfo->itemList, item);
				addAmount = FMath::RandRange(structureInfo->itemSellListId[index].sellingItemMinAmount, structureInfo->itemSellListId[index].sellingItemMaxAmount);

				if (findIndex > -1)
					structureInfo->itemList[findIndex].itemAmount += addAmount;
				else if (addAmount > 0)
					structureInfo->itemList.Emplace(FItem(structureInfo->itemSellListId[index].sellingItemID, addAmount));
			}
			else {
				item = FItem(structureInfo->itemSellListId[index].sellingItemID, 1);
				findIndex = USafeENGINE::FindItemSlot(structureInfo->itemList, item);

				if (findIndex > -1)
					structureInfo->itemList.RemoveAtSwap(findIndex);
			}
		}
		structureInfo->remainItemListRefreshTime = structureInfo->maxItemListRefreshTime;
		if (Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->DockedStructure().GetObjectRef() == this->_getUObject()) {
			UE_LOG(LogClass, Log, TEXT("[Info][Gate][Tick] Update UI with sell refresh"));
			Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationInfo();
		}
	}
	
	if ((structureInfo->structureType == StructureType::ProductionFacility || structureInfo->structureType == StructureType::Hub) && structureInfo->productItemList.Num() > 0) {
		structureInfo->remainProductTime = FMath::Clamp(structureInfo->remainProductTime - DeltaSeconds, 0.0f, structureInfo->maxItemListRefreshTime);
		if (structureInfo->remainProductTime <= 0.0f) {
			bool canProduct = true;
			int findIndex = -1;

			for (int index = 0; index < structureInfo->consumptItemList.Num(); index++) {
				findIndex = USafeENGINE::FindItemSlot(structureInfo->itemList, structureInfo->consumptItemList[index]);

				if (findIndex < 0 || structureInfo->itemList[findIndex].itemAmount < structureInfo->consumptItemList[index].itemAmount)
				{
					canProduct = false;
					break;
				}
			}

			if (canProduct == true) {
				for (int index = 0; index < structureInfo->consumptItemList.Num(); index++) {
					findIndex = USafeENGINE::FindItemSlot(structureInfo->itemList, structureInfo->consumptItemList[index]);
					structureInfo->itemList[findIndex].itemAmount -= structureInfo->consumptItemList[index].itemAmount;
					if (structureInfo->itemList[findIndex].itemAmount <= 0)
						structureInfo->itemList.RemoveAt(findIndex);
				}

				for (int index = 0; index < structureInfo->productItemList.Num(); index++) {
					if (!canProduct || index > structureInfo->maxProductAmount.Num() || structureInfo->itemList[findIndex].itemAmount >= structureInfo->maxProductAmount[index])
						break;

					findIndex = USafeENGINE::FindItemSlot(structureInfo->itemList, structureInfo->productItemList[index]);
					if (findIndex > -1) 
						structureInfo->itemList[findIndex].itemAmount += structureInfo->productItemList[index].itemAmount;
					else 
						structureInfo->itemList.Emplace(FItem(structureInfo->productItemList[index]));
				}
				structureInfo->remainProductTime = structureInfo->maxProductTime;
				if (Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->DockedStructure().GetObjectRef() == this->_getUObject()) {
					UE_LOG(LogClass, Log, TEXT("[Info][Gate][Tick] Update UI with processing"));
					Cast<ASpaceHUDBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->UpdateUIStationInfo();
				}
			}
		}
	}
}

float AStation::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) {
	if (!DamageCauser->IsA(ASpaceObject::StaticClass()))
		return 0.0f;

	float remainDamage = DamageAmount * FMath::FRandRange(0.85f, 1.15f);

	float effectShieldDamage = 0.0f;
	float effectArmorDamage = 0.0f;
	float effectHullDamage = 0.0f;
	bool isCritical = false;

	FHitResult hitResult;
	FVector hitDirect;
	DamageEvent.GetBestHitInfo(this, DamageCauser, hitResult, hitDirect);
	hitDirect = hitResult.ImpactPoint - GetActorLocation();
	hitDirect.Normalize();

	if (FVector::DotProduct(GetActorForwardVector(), hitDirect) > 0.95f) {
		remainDamage *= 2.0f;
		isCritical = true;
	}
	else if (FVector::DotProduct(GetActorForwardVector(), hitDirect) < -0.95f) {
		remainDamage *= 3.0f;
		isCritical = true;
	}

	//remainDamage = sDefShield.GetValue();
	if (currentShield > remainDamage) {
		effectShieldDamage = remainDamage;
		currentShield -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectShieldDamage = currentShield;
		remainDamage -= effectShieldDamage;
		currentShield = 0.0f;
	}

	if (currentArmor > remainDamage) {
		effectArmorDamage = remainDamage;
		currentArmor -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectArmorDamage = currentArmor;
		remainDamage -= effectArmorDamage;
		currentArmor = 0.0f;
	}

	if (currentHull > remainDamage) {
		effectHullDamage = remainDamage;
		currentHull -= remainDamage;
		remainDamage = 0.0f;
	}
	else {
		effectHullDamage = currentHull;
		currentHull = 0.0f;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, this->GetName() + " is Die!");
	}

	UE_LOG(LogClass, Log, TEXT("[Info][PlayerShip][Damaged] %s Get %s Type of %.0f Damage From %s! Effect Damage : Shield - %.0f / Armor - %.0f / Hull - %.0f. is Critical Damage? : %s"), *this->GetName(), *DamageEvent.DamageTypeClass->GetName(), remainDamage, *DamageCauser->GetName(), effectShieldDamage, effectArmorDamage, effectHullDamage, isCritical ? TEXT("Critical") : TEXT("Non Critical"));

	return effectShieldDamage + effectArmorDamage + effectHullDamage;
}

void AStation::BeginDestroy() {
	UnregisterAllComponents();
	Super::BeginDestroy();
}
#pragma endregion

#pragma region SpaceObject Inheritance
int AStation::GetObjectID() {
	if(structureInfo != nullptr)
		return structureInfo->structureID; 
	else return -1;
}

ObjectType AStation::GetObjectType() {
	return ObjectType::Station;
}

Faction AStation::GetFaction() {
	if (structureInfo != nullptr)
		return structureInfo->structureFaction; 
	else return Faction::Neutral;
}

void AStation::SetFaction(Faction setFaction) {
	return;
}

BehaviorState AStation::GetBehaviorState() {
	return BehaviorState::Idle;
}

bool AStation::InitObject(int objectId) {
	return false;
}

bool AStation::LoadBaseObject(float shield, float armor, float hull, float power) {
	return false;
}

float AStation::GetValue(GetStatType statType) {
	float value;

	switch (statType) {
	case GetStatType::halfLength:
		value = lengthToLongAsix * 0.5f;
		break;
	case GetStatType::maxShield:
		value = maxShield;
		break;
	case GetStatType::rechargeShield:
		value = rechargeShield;
		break;
	case GetStatType::currentShield:
		value = currentShield;
		break;
	case GetStatType::defShield:
		value = defShield;
		break;

	case GetStatType::maxArmor:
		value = maxArmor;
		break;
	case GetStatType::repaireArmor:
		value = repairArmor;
		break;
	case GetStatType::currentArmor:
		value = currentArmor;
		break;
	case GetStatType::defArmor:
		value = defArmor;
		break;

	case GetStatType::maxHull:
		value = maxHull;
		break;
	case GetStatType::repaireHull:
		value = repairHull;
		break;
	case GetStatType::currentHull:
		value = currentHull;
		break;
	case GetStatType::defHull:
		value = defHull;
		break;
	default:
		value = -1;
		break;
	}
	return value;
}

void AStation::GetRepaired(GetStatType statType, float repairValue) {

	repairValue = FMath::Clamp(repairValue, 0.0f, 500.0f);
	switch (statType) {
	case GetStatType::currentShield:
		currentShield = FMath::Clamp(currentShield + repairValue, 0.0f, maxShield);
		break;
	case GetStatType::currentArmor:
		currentArmor = FMath::Clamp(currentArmor + repairValue, 0.0f, maxArmor);
		break;
	case GetStatType::currentHull:
		currentHull = FMath::Clamp(currentHull + repairValue, 0.0f, maxHull);
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region Interface Implementation : IStructureable
FString AStation::GetDestinationName() {
	return "";
}

StructureType AStation::GetStationType() {
	return structureInfo->structureType;
}

bool AStation::RequestedDock(Faction requestFaction) {
	return true;
}

bool AStation::RequestedJump(Faction requestFaction) {
	return false;
}

bool AStation::SetStructureData(FStructureInfo& structureData) {
	if (isInited)
		return false;

	USafeENGINE* tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FStationData tempStationData;

	structureInfo = &structureData;
	tempStationData = tempInstance->GetStationData((structureInfo->structureID));

	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *tempStationData.MeshPath.ToString()));
	objectMesh->SetStaticMesh(newMesh);
	lengthToLongAsix = tempStationData.lengthToLongAsix;

	maxShield = tempStationData.Shield;
	currentShield = structureInfo->structureShieldRate * maxShield;
	rechargeShield = tempStationData.RechargeShield;
	defShield = tempStationData.DefShield;

	maxArmor = tempStationData.Armor;
	currentArmor = structureInfo->structureArmorRate * maxArmor;
	repairArmor = tempStationData.RepairArmor;
	defArmor = tempStationData.DefArmor;

	maxHull = tempStationData.Hull;
	currentHull = structureInfo->structureHullRate * maxHull;
	repairHull = tempStationData.RepairHull;
	defHull = tempStationData.DefHull;

	AddActorWorldOffset(FVector(0.0f, 0.0f, 1.0f));
	isInited = true;
	return true;
}

void AStation::GetStructureData(FStructureInfo& structureData) {
	structureData = *structureInfo;
	return;
}
#pragma endregion

#pragma region Functions
FStructureInfo* AStation::GetStructureDataPointer() {
	return structureInfo;
}

void AStation::RefreshStationItem(){

}
void AStation::ProductItemInStationCargo(){

}
void AStation::ProductItemInPlayerCargo(){

}
#pragma endregion