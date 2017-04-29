// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
/**
 *  Relative to data table, First characher is big alphabet...
 */

#pragma region Game Data
UENUM(BlueprintType)
enum class Faction : uint8 {
	Neutral					UMETA(DisplayName = "Neutral"),
	Player					UMETA(DisplayName = "Player"),
	PlayerFoundingFaction		UMETA(DisplayName = "Founded Faction By Player"),

	PrimusEmpire			UMETA(DisplayName = "Primus Empire"),
	FlorenceTradeCoalition	UMETA(DisplayName = "Florence Trade Coalition"),
	FreeCitizenFederation	UMETA(DisplayName = "Free Citizen Federation"),
	ValenciaProtectorate	UMETA(DisplayName = "Valencia Protectorate"),
	HeartOfLiberty		UMETA(DisplayName = "Heart Of Liberty"),
	Pirate			UMETA(DisplayName = "Pirate")

};
UENUM(BlueprintType)
enum class Peer : uint8 {
	EnemyStrong		UMETA(DisplayName = "EnemyStrong"),
	Enemy			UMETA(DisplayName = "Enemy"),
	Boundary		UMETA(DisplayName = "Boundary"),
	TempHold		UMETA(DisplayName = "TempHold"),
	Neutral			UMETA(DisplayName = "Neutral"),
	Friendship		UMETA(DisplayName = "Friendship"),
	Ally			UMETA(DisplayName = "Ally"),
	AllyStrong		UMETA(DisplayName = "AllyStrong")
};
UENUM(BlueprintType)
enum class ObjectType : uint8 {
	Etc			UMETA(DisplayName = "Etc"),
	Ship			UMETA(DisplayName = "Ship"),
	Station			UMETA(DisplayName = "Station"),
	Gate			UMETA(DisplayName = "Gate"),
	Drone			UMETA(DisplayName = "Drone"),
	SpaceObject		UMETA(DisplayName = "SpaceObject"),
	Container		UMETA(DisplayName = "Container"),
	Resource		UMETA(DisplayName = "Resource")
};
UENUM(BlueprintType)
enum class ResourceType : uint8 {
	Asteroid		UMETA(DisplayName = "Asteroid"),
	Gas			UMETA(DisplayName = "Gas"),
	Liquid			UMETA(DisplayName = "Liquid")
};
UENUM(BlueprintType)
enum class ShipClass : uint8 {
	Frigate			UMETA(DisplayName = "Frigate"),
	Destroyer		UMETA(DisplayName = "Destroyer"),
	Cruiser			UMETA(DisplayName = "Cruiser"),
	BattleCruiser		UMETA(DisplayName = "BattleCruiser"),
	BattleShip		UMETA(DisplayName = "BattleShip"),
	Carrier			UMETA(DisplayName = "Carrier"),

	TransfortShip		UMETA(DisplayName = "TransfortShip"),
	Freighter		UMETA(DisplayName = "Freighter"),

	MiningShip		UMETA(DisplayName = "MiningShip"),
	MiningBarge		UMETA(DisplayName = "MiningBarge"),
	Hulk			UMETA(DisplayName = "Hulk")
};
UENUM(BlueprintType)
enum class DroneClass : uint8 {
	Light			UMETA(DisplayName = "Light"),
	Midium			UMETA(DisplayName = "Midium"),
	Heavy			UMETA(DisplayName = "Heavy"),
	Fighter			UMETA(DisplayName = "Fighter"),
	Bomber			UMETA(DisplayName = "Bomber"),
	Gunship			UMETA(DisplayName = "Gunship"),
	Corvette		UMETA(DisplayName = "Corvette")
};
UENUM(BlueprintType)
enum class DroneRuleType : uint8 {
	Combat				UMETA(DisplayName = "Combat"),
	Logi_Shield			UMETA(DisplayName = "Logi_Shield"),
	Logi_Armor			UMETA(DisplayName = "Logi_Armor"),
	Logi_Hull			UMETA(DisplayName = "Logi_Hull"),
	Transforter			UMETA(DisplayName = "Transforter"),
	Mining				UMETA(DisplayName = "Mining")
};
UENUM(BlueprintType)
enum class StructureType : uint8 {
	Gate				UMETA(DisplayName = "Gate"),
	TradingCenter			UMETA(DisplayName = "TradingCenter"),
	Hub				UMETA(DisplayName = "Hub"),
	Shipyard			UMETA(DisplayName = "Shipyard"),
	ProductionFacility		UMETA(DisplayName = "ProductionFacility"),
	MilitaryFacility		UMETA(DisplayName = "MilitaryFacility"),
};
UENUM(BlueprintType)
enum class BehaviorState : uint8 {
	Idle				UMETA(DisplayName = "Idle"),
	Move				UMETA(DisplayName = "Move"),
	Patrol				UMETA(DisplayName = "Patrol"),
	Docking				UMETA(DisplayName = "Docking"),
	Docked				UMETA(DisplayName = "Docked"),
	Battle				UMETA(DisplayName = "Battle"),
	Mining				UMETA(DisplayName = "Mining"),
	Repair				UMETA(DisplayName = "Repair"),
	Warping				UMETA(DisplayName = "Warping"),
	Jumping				UMETA(DisplayName = "Jumping")
};
UENUM(BlueprintType)
enum class BehaviorType : uint8 {
	Trader				UMETA(DisplayName = "Trader"),
	Miner				UMETA(DisplayName = "Miner"),
	Passive				UMETA(DisplayName = "Passive"),
	Active				UMETA(DisplayName = "Active"),
	Rage				UMETA(DisplayName = "Rage")
};

UENUM(BlueprintType)
enum class GetStatType : uint8 {
	Etc				UMETA(DisplayName = "Etc"),
	halfLength			UMETA(DisplayName = "halfLength"),
	raderDistance			UMETA(DisplayName = "raderDistance"),
	engageDistance			UMETA(DisplayName = "engageDistance"),
	bounty				UMETA(DisplayName = "bounty"),

	maxShield			UMETA(DisplayName = "maxShield"),
	rechargeShield			UMETA(DisplayName = "rechargeShield"),
	currentShield			UMETA(DisplayName = "currentShield"),
	defShield			UMETA(DisplayName = "defShield"),

	maxArmor			UMETA(DisplayName = "maxArmor"),
	repaireArmor			UMETA(DisplayName = "repaireArmor"),
	currentArmor			UMETA(DisplayName = "currentArmor"),
	defArmor			UMETA(DisplayName = "defArmor"),

	maxHull				UMETA(DisplayName = "maxHull"),
	repaireHull			UMETA(DisplayName = "repaireHull"),
	currentHull			UMETA(DisplayName = "currentHull"),
	defHull				UMETA(DisplayName = "defHull"),

	maxPower			UMETA(DisplayName = "maxPower"),
	rechargePower			UMETA(DisplayName = "rechargePower"),
	currentPower			UMETA(DisplayName = "currentPower"),

	maxCompute			UMETA(DisplayName = "maxCompute"),
	currentCompute			UMETA(DisplayName = "currentCompute"),
	maxPowerGrid			UMETA(DisplayName = "maxPowerGrid"),
	currentPowerGrid		UMETA(DisplayName = "currentPowerGrid"),
	maxCargo			UMETA(DisplayName = "maxCargo"),

	maxSpeed			UMETA(DisplayName = "maxSpeed"),
	targetSpeed			UMETA(DisplayName = "targetSpeed"),
	currentSpeed			UMETA(DisplayName = "currentSpeed"),

	maxAcceleration			UMETA(DisplayName = "maxAcceleration"),
	minAcceleration			UMETA(DisplayName = "minAcceleration"),
	currentAcceleration		UMETA(DisplayName = "currentAcceleration"),

	maxRotateRate			UMETA(DisplayName = "maxRotateRate"),
	rotateAcceleraion		UMETA(DisplayName = "rotateAcceleraion"),
	rotateDeceleraion		UMETA(DisplayName = "rotateDeceleraion"),
	currentRotateRate		UMETA(DisplayName = "currentRotateRate"),

	droneBaseStatsMultiple		UMETA(DisplayName = "droneBaseStatsMultiple"),
	droneDroneDamageMultiple	UMETA(DisplayName = "droneDroneDamageMultiple"),
	droneDroneRangeMultiple		UMETA(DisplayName = "droneDroneRangeMultiple"),
	droneDroneSpeedMultiple		UMETA(DisplayName = "droneDroneSpeedMultiple"),
	droneDroneControlMultiple	UMETA(DisplayName = "droneDroneControlMultiple")
};
UENUM(BlueprintType)
enum class BonusStatType : uint8 {
	CustomBonus			UMETA(DisplayName = "CustomBonus"),

	BonusMaxShield			UMETA(DisplayName = "BonusMaxShield"),
	BonusRechargeShield		UMETA(DisplayName = "BonusRechargeShield"),
	BonusDefShield			UMETA(DisplayName = "BonusDefShield"),

	BonusMaxArmor			UMETA(DisplayName = "BonusMaxArmor"),
	BonusRepaireArmor			UMETA(DisplayName = "BonusRepaireArmor"),
	BonusDefArmor			UMETA(DisplayName = "BonusDefArmor"),

	BonusMaxHull			UMETA(DisplayName = "BonusMaxHull"),
	BonusRepaireHull		UMETA(DisplayName = "BonusRepaireHull"),
	BonusDefHull			UMETA(DisplayName = "BonusFefHull"),

	BonusMaxPower			UMETA(DisplayName = "BonusMaxPower"),
	BonusRechargePower		UMETA(DisplayName = "BonusRechargePower"),

	BonusMaxRadarRange			UMETA(DisplayName = "MaxRadarRange"),
	BonusMaxCPUPerfomance			UMETA(DisplayName = "BonusMaxCPUPerfomance"),
	BonusMaxPowerGridPerfomance		UMETA(DisplayName = "BonusMaxPowerGridPerfomance"),
	BonusMaxCargoSize		UMETA(DisplayName = "BonusMaxCargoSize"),
	
	BonusMobilitySpeed		UMETA(DisplayName = "BonusMobilitySpeed"),
	BonusMobilityAcceleration	UMETA(DisplayName = "BonusMobilityAcceleration"),
	BonusMobilityRotation		UMETA(DisplayName = "BonusMobilityRotation"),
	BonusMobilityRotateAcceleration	UMETA(DisplayName = "BonusMobilityRotateAcceleration"),

	BonusActiveRechargeShield	UMETA(DisplayName = "BonusActiveRechargeShield"),
	BonusActiveDefShield		UMETA(DisplayName = "BonusActiveDefShield"),
	BonusActiveRepaireArmor	UMETA(DisplayName = "BonusActiveRepaireArmor"),
	BonusActiveDefArmor		UMETA(DisplayName = "BonusActiveDefArmor"),
	BonusActiveRepaireHull		UMETA(DisplayName = "BonusActiveRepaireHull"),
	BonusActiveDefHull		UMETA(DisplayName = "BonusActiveDefHull"),

	BonusActiveSpeed		UMETA(DisplayName = "BonusActiveSpeed"),
	BonusActiveAcceleration		UMETA(DisplayName = "BonusActiveAcceleration"),
	BonusActiveRotation		UMETA(DisplayName = "BonusActiveRotation"),

	BonusActiveModuleUsagePower		UMETA(DisplayName = "BonusActiveModuleUsagePower"),
	BonusTargetModuleUsagePower		UMETA(DisplayName = "BonusTargetModuleUsagePower"),

	BonusBeamDamage			UMETA(DisplayName = "BonusBeamDamage"),
	BonusBeamPower			UMETA(DisplayName = "BonusBeamPower"),
	BonusBeamCoolTime		UMETA(DisplayName = "BonusBeamCoolTime"),
	BonusBeamAccuracy		UMETA(DisplayName = "BonusBeamAccuracy"),
	BonusBeamRange			UMETA(DisplayName = "BonusBeamRange"),

	BonusCannonDamage		UMETA(DisplayName = "BonusCannonDamage"),
	BonusCannonCoolTime		UMETA(DisplayName = "BonusCannonCoolTime"),
	BonusCannonAccuracy		UMETA(DisplayName = "BonusCannonAccuracy"),
	BonusCannonLifeTime		UMETA(DisplayName = "BonusCannonLifeTime"),
	BonusCannonLaunchVelocity	UMETA(DisplayName = "BonusCannonLaunchVelocity"),

	BonusRailGunDamage		UMETA(DisplayName = "BonusRailGunDamage"),
	BonusRailGunPower		UMETA(DisplayName = "BonusRailGunPower"),
	BonusRailGunCoolTime		UMETA(DisplayName = "BonusRailGunCoolTime"),
	BonusRailGunAccuracy		UMETA(DisplayName = "BonusRailGunAccuracy"),
	BonusRailGunLifeTime		UMETA(DisplayName = "BonusRailGunLifeTime"),
	BonusRailGunLaunchVelocity	UMETA(DisplayName = "BonusRailGunLaunchVelocity"),

	BonusMissileDamage		UMETA(DisplayName = "BonusMissileDamage"),
	BonusMissileCoolTime		UMETA(DisplayName = "BonusMissileCoolTime"),
	BonusMissileAccuracy		UMETA(DisplayName = "BonusMissileAccuracy"),
	BonusMissileLifeTime		UMETA(DisplayName = "BonusMissileLifeTime"),
	BonusMissileLaunchVelocity	UMETA(DisplayName = "BonusMissileLaunchVelocity"),

	BonusDroneBaseStats		UMETA(DisplayName = "BonusDroneBaseStats"),
	BonusDroneControl		UMETA(DisplayName = "BonusDroneControl"),
	BonusDroneDamage		UMETA(DisplayName = "BonusDroneDamage"),
	BonusDroneRange			UMETA(DisplayName = "BonusDroneRange"),
	BonusDroneSpeed			UMETA(DisplayName = "BonusDroneSpeed"),

	BonusMiningAmount		UMETA(DisplayName = "BonusMiningAmount"),
	BonusMiningPower		UMETA(DisplayName = "BonusMiningPower"),
	BonusMiningCoolTime		UMETA(DisplayName = "BonusMiningCooltime"),
	BonusMiningRange		UMETA(DisplayName = "BonusMiningRange"),

	BonusReprocessingSlot		UMETA(DisplayName = "BonusReprocessingSlot"),
	BonusReprocessingTime		UMETA(DisplayName = "BonusReprocessingTime"),
	BonusReprocessingAmount		UMETA(DisplayName = "BonusReprocessingAmount")
};

UENUM(BlueprintType)
enum class ItemType : uint8 {
	Etc				UMETA(DisplayName = "Etc"),
	TargetModule			UMETA(DisplayName = "TargetModule"),
	ActiveModule			UMETA(DisplayName = "ActiveModule"),
	PassiveModule			UMETA(DisplayName = "PassiveModule"),
	SystemModule			UMETA(DisplayName = "SystemModule"),

	Ore				UMETA(DisplayName = "Ore"),
	Mineral				UMETA(DisplayName = "Mineral"),
	IndustryParts			UMETA(DisplayName = "IndustryParts"),
	ConstructPart			UMETA(DisplayName = "ConstructPart"),
	Blueprint			UMETA(DisplayName = "Blueprint"),
	Trade				UMETA(DisplayName = "Trade"),

	Ammo_Shell			UMETA(DisplayName = "Shell"),
	Ammo_Sabot			UMETA(DisplayName = "Sabot"),
	Ammo_Missile			UMETA(DisplayName = "Missile"),
	Quest				UMETA(DisplayName = "Quest")
};
UENUM(BlueprintType)
enum class ProjectileType : uint8 {
	Shell			UMETA(DisplayName = "Shell"),
	Sabot			UMETA(DisplayName = "Sabot"),
	Missile			UMETA(DisplayName = "Missile"),
};
UENUM(BlueprintType)
enum class ModuleType : uint8 {
	NotModule			UMETA(DisplayName = "NotModule"),

	Beam				UMETA(DisplayName = "Beam"),
	Cannon				UMETA(DisplayName = "Cannon"),
	Railgun				UMETA(DisplayName = "Railgun"),
	MissileLauncher			UMETA(DisplayName = "MissileLauncher"),

	MinerLaser			UMETA(DisplayName = "MinerLaser"),
	TractorBeam			UMETA(DisplayName = "TractorBeam"),

	ShieldRegenLaser		UMETA(DisplayName = "ShieldRegenLaser"),
	ArmorRepairLaser		UMETA(DisplayName = "ArmorRepairLaser"),
	HullRepairLaser			UMETA(DisplayName = "HullRepairLaser"),
	
	ShieldGenerator			UMETA(DisplayName = "ShieldGenerator"),
	ShieldEnhancer			UMETA(DisplayName = "ShieldEnhancer"),
	ArmorRepairer			UMETA(DisplayName = "ArmorRepairer"),
	ArmorEnhancer			UMETA(DisplayName = "ArmorEnhancer"),
	HullRepairer			UMETA(DisplayName = "HullRepairer"),
	HullEnhancer			UMETA(DisplayName = "HullEnhancer"),

	EngineController		UMETA(DisplayName = "EngineController"),
	Accelerator				UMETA(DisplayName = "Accelerator"),
	SteeringController		UMETA(DisplayName = "SteeringController"),

	PassiveModule			UMETA(DisplayName = "PassiveModule"),
	SystemModule			UMETA(DisplayName = "SystemModule")
};
UENUM(BlueprintType)
enum class ModuleSize : uint8 {
	NotSized			UMETA(DisplayName = "NotSized"),

	Small				UMETA(DisplayName = "Small"),
	Medium				UMETA(DisplayName = "Medium"),
	Large				UMETA(DisplayName = "Large"),
	XLarge				UMETA(DisplayName = "XLarge"),
	Capital				UMETA(DisplayName = "Capital")
};
UENUM(BlueprintType)
enum class ModuleState : uint8 {
	NotActivate			UMETA(DisplayName = "NotActivate"),
	Activate				UMETA(DisplayName = "Activate"),
	Overload				UMETA(DisplayName = "Overload"),
	ReloadAmmo				UMETA(DisplayName = "ReloadAmmo")
};

UENUM(BlueprintType)
enum class Region : uint8 {
	UnknownRegionArea_Unclassified		UMETA(DisplayName = "Unknown Region Area Unclassified"),
	Aldir							UMETA(DisplayName = "Aldir"),
	Debrak							UMETA(DisplayName = "Debrak"),
	Cannide							UMETA(DisplayName = "Cannide"),
	Centaur							UMETA(DisplayName = "Centaur"),
	Tashima							UMETA(DisplayName = "Tashima"),

	ElgishShima					UMETA(DisplayName = "Elgish Shima"),
	Hansmit							UMETA(DisplayName = "Hansmit"),
	DomainOfWorld					UMETA(DisplayName = "Domain Of World"),
	CenterOfWorld				UMETA(DisplayName = "Center Of World"),

	UnknownRegionArea_A			UMETA(DisplayName = "Unknown Region Area A"),
	UnknownRegionArea_B			UMETA(DisplayName = "Unknown Region Area B"),
	UnknownRegionArea_C			UMETA(DisplayName = "Unknown Region Area C")
};
UENUM(BlueprintType)
enum class SectorState : uint8 {
	Normal				UMETA(DisplayName = "Normal"),
	AcitveBattle			UMETA(DisplayName = "AcitveBattle"),
	AcitveDevelop			UMETA(DisplayName = "AcitveDevelop"),
	AcitveTrading			UMETA(DisplayName = "AcitveTrading"),
	ActiveSecurity			UMETA(DisplayName = "ActiveSecurity"),
	ActiveResearch			UMETA(DisplayName = "ActiveResearch"),

	DeacitveDevelop			UMETA(DisplayName = "DeacitveDevelop"),
	DeacitveTrading			UMETA(DisplayName = "DeacitveTrading"),
	DeactiveSecurity		UMETA(DisplayName = "DeactiveSecurity"),
	DeactiveResearch		UMETA(DisplayName = "DeactiveResearch")
};
UENUM(BlueprintType)
enum class SectorType : uint8 {
	NonInfo				UMETA(DisplayName = "NonInfo"),
	Outer				UMETA(DisplayName = "Outer"),
	Border				UMETA(DisplayName = "Border"),
	Normal				UMETA(DisplayName = "Normal"),
	Gate				UMETA(DisplayName = "Gate"),
	Central				UMETA(DisplayName = "Central"),
	Capital				UMETA(DisplayName = "Capital")
};
#pragma endregion

#pragma region Game Play State
UENUM(BlueprintType)
enum class InteractionType : uint8 {
	Info				UMETA(DisplayName = "Info"),
	Communication			UMETA(DisplayName = "Communication"),
	DockRequest			UMETA(DisplayName = "DockRequest"),
	Approach			UMETA(DisplayName = "Approach"),
	Attack				UMETA(DisplayName = "Attack"),
	Jump				UMETA(DisplayName = "Jump"),
	Warp				UMETA(DisplayName = "Warp"),
	Collect				UMETA(DisplayName = "Collect"),
	Repair				UMETA(DisplayName = "Repair")
};
UENUM(BlueprintType)
enum class ConfirmState : uint8 {
	Idle				UMETA(DisplayName = "Idle"),
	WaitInMenu_Save			UMETA(DisplayName = "WaitInMenu_Save"),
	WaitInMenu_Load			UMETA(DisplayName = "WaitInMenu_Load"),
	WaitInMenu_ReturnToTitle	UMETA(DisplayName = "WaitInMenu_ReturnToTitle"),
	WaitInMenu_GameExit		UMETA(DisplayName = "WaitInMenu_GameExit")
};
UENUM(BlueprintType)
enum class SaveState : uint8 {
	NewGameCreate			UMETA(DisplayName = "NewGameCreate"),
	UserRequest			UMETA(DisplayName = "UserRequest"),
	BeforeWarp			UMETA(DisplayName = "BeforeWarp"),
};
#pragma endregion