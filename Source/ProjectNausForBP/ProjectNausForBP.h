// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

#include "GameCore/MainTitleMode.h"
#include "GameCore/SpaceMode.h"
#include "GameCore/SpaceState.h"
#include "GameCore/UserController.h"
#include "GameCore/MainTitleController.h"
#include "GameCore/SpaceAIController.h"
#include "GameCore/UserState.h"
#include "GameCore/SpaceHUDBase.h"

#include "GameCoreSub/SafeENGINE.h"
#include "GameCoreSub/SaveLoader.h"
#include "GameCoreSub/Enums.h"

#include "GameObject/PlayerShip.h"
#include "GameObject/Ship.h"
#include "GameObject/Drone.h"
#include "GameObject/Station.h"
#include "GameObject/Gate.h"
#include "GameObject/Resource.h"
#include "GameObject/CargoContainer.h"
#include "GameObject/SpaceObject.h"

#include "Interface/Commandable.h"
#include "Interface/Structureable.h"
#include "Interface/BehaviorList.h"

#include "Projectile/Beam.h"
#include "Projectile/Projectiles.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"