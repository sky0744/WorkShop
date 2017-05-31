// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
/**
 *  Relative to data table, First characher is big alphabet...
 */

#pragma region Game Data
UENUM(BlueprintType)
enum class Faction : uint8 {
	Neutral						UMETA(DisplayName = "중립"),
	Player						UMETA(DisplayName = "플레이어"),
	PlayerFoundingFaction		UMETA(DisplayName = "플레이어 팩션"),

	Empire						UMETA(DisplayName = "제국"),
	TradeCoalition				UMETA(DisplayName = "무역연합"),
	CitizenFederation			UMETA(DisplayName = "시민연방"),
	Protectorate				UMETA(DisplayName = "보호령"),
	LibertyNation		UMETA(DisplayName = "자유국"),
	Pirate						UMETA(DisplayName = "해적")
};
UENUM(BlueprintType)
enum class Peer : uint8 {
	EnemyStrong		UMETA(DisplayName = "강한 적대"),
	Enemy			UMETA(DisplayName = "적대"),
	Boundary		UMETA(DisplayName = "경계"),
	TempHold		UMETA(DisplayName = "판단 보류"),
	Neutral			UMETA(DisplayName = "중립"),
	Friendship		UMETA(DisplayName = "우호"),
	Ally			UMETA(DisplayName = "동맹"),
	AllyStrong		UMETA(DisplayName = "강한 동맹")
};
UENUM(BlueprintType)
enum class ObjectType : uint8 {
	Etc			UMETA(DisplayName = "기타"),
	Ship			UMETA(DisplayName = "함선"),
	Station			UMETA(DisplayName = "스테이션"),
	Gate			UMETA(DisplayName = "게이트"),
	Drone			UMETA(DisplayName = "드론"),
	SpaceObject		UMETA(DisplayName = "물체"),
	Container		UMETA(DisplayName = "컨테이너"),
	Resource		UMETA(DisplayName = "자원")
};
UENUM(BlueprintType)
enum class ResourceType : uint8 {
	Asteroid		UMETA(DisplayName = "소행성-광물"),
	Gas			UMETA(DisplayName = "가스 자원"),
	Liquid			UMETA(DisplayName = "액화 자원")
};
UENUM(BlueprintType)
enum class ShipClass : uint8 {
	Frigate			UMETA(DisplayName = "프리깃"),
	Destroyer		UMETA(DisplayName = "구축함"),
	Cruiser			UMETA(DisplayName = "순양함"),
	BattleCruiser		UMETA(DisplayName = "전투순양함"),
	BattleShip		UMETA(DisplayName = "전함"),
	Carrier			UMETA(DisplayName = "우주모함"),

	TransfortShip		UMETA(DisplayName = "수송선박"),
	Freighter		UMETA(DisplayName = "대형 수송선박"),

	MiningShip		UMETA(DisplayName = "채광선박"),
	MiningBarge		UMETA(DisplayName = "특수 자원함"),
	Hulk			UMETA(DisplayName = "대형 채광선박")
};
UENUM(BlueprintType)
enum class DroneClass : uint8 {
	Light			UMETA(DisplayName = "소형급"),
	Medium			UMETA(DisplayName = "중형급"),
	Heavy			UMETA(DisplayName = "대형급"),
	Intercepter		UMETA(DisplayName = "요격기"),
	Bomber			UMETA(DisplayName = "폭격기"),
	Gunship			UMETA(DisplayName = "포격기")
};
UENUM(BlueprintType)
enum class DroneRuleType : uint8 {
	Combat				UMETA(DisplayName = "전투"),
	Logi_Shield			UMETA(DisplayName = "로지스틱스"),
	Logi_Armor			UMETA(DisplayName = "로지스틱스"),
	Logi_Hull			UMETA(DisplayName = "로지스틱스"),
	Transforter			UMETA(DisplayName = "운반"),
	Mining				UMETA(DisplayName = "채광")
};
UENUM(BlueprintType)
enum class StructureType : uint8 {
	Gate				UMETA(DisplayName = "게이트"),
	TradingCenter			UMETA(DisplayName = "무역센터"),
	Hub				UMETA(DisplayName = "허브"),
	Shipyard			UMETA(DisplayName = "조선소"),
	ProductionFacility		UMETA(DisplayName = "생산시설"),
	MilitaryFacility		UMETA(DisplayName = "군용시설"),
};
UENUM(BlueprintType)
enum class BehaviorState : uint8 {
	Idle				UMETA(DisplayName = "대기 중"),
	Move				UMETA(DisplayName = "이동 중"),
	Patrol				UMETA(DisplayName = "순찰 중"),
	Docking				UMETA(DisplayName = "도킹 중"),
	Docked				UMETA(DisplayName = "도킹 완료"),
	Battle				UMETA(DisplayName = "전투 중"),
	Mining				UMETA(DisplayName = "채광 중"),
	Repair				UMETA(DisplayName = "수리 중"),
	Warping				UMETA(DisplayName = "워프 중"),
	Jumping				UMETA(DisplayName = "점프 중")
};
UENUM(BlueprintType)
enum class BehaviorType : uint8 {
	Trader				UMETA(DisplayName = "AI_무역"),
	Miner				UMETA(DisplayName = "AI_광부"),
	Standby				UMETA(DisplayName = "AI_위치사수"),
	Passive				UMETA(DisplayName = "AI_비선공"),
	Active				UMETA(DisplayName = "AI_선공"),
	Rage				UMETA(DisplayName = "AI_탐색 및 선공")
};

UENUM(BlueprintType)
enum class GetStatType : uint8 {
	Etc					UMETA(DisplayName = "기타"),

	raderDistance			UMETA(DisplayName = "탐지 거리"),
	engageDistance			UMETA(DisplayName = "교전 거리"),
	bounty					UMETA(DisplayName = "현상금"),

	maxShield				UMETA(DisplayName = "최대 쉴드"),
	rechargeShield			UMETA(DisplayName = "쉴드 재생"),
	currentShield			UMETA(DisplayName = "현재 쉴드"),
	defShield				UMETA(DisplayName = "쉴드 방어"),

	maxArmor				UMETA(DisplayName = "최대 장갑"),
	repaireArmor			UMETA(DisplayName = "장갑 수리"),
	currentArmor			UMETA(DisplayName = "현재 장갑"),
	defArmor				UMETA(DisplayName = "장갑 방어"),

	maxHull					UMETA(DisplayName = "최대 선체"),
	repaireHull				UMETA(DisplayName = "선체 수리"),
	currentHull				UMETA(DisplayName = "현재 선체"),
	defHull					UMETA(DisplayName = "선체 방어"),

	maxPower				UMETA(DisplayName = "최대 전력"),
	rechargePower			UMETA(DisplayName = "전력 생산"),
	currentPower			UMETA(DisplayName = "현재 전력"),

	maxCompute				UMETA(DisplayName = "최대 연산능력"),
	currentCompute			UMETA(DisplayName = "현재 연산능력"),
	maxPowerGrid			UMETA(DisplayName = "최대 전력망"),
	currentPowerGrid		UMETA(DisplayName = "현재 전력망"),
	maxCargo				UMETA(DisplayName = "최대 카고용적"),

	warpMaxSpeed			UMETA(DisplayName = "최대 워프속도"),
	warpAcceleration		UMETA(DisplayName = "워프 가속"),
	warpCurrentSpeed		UMETA(DisplayName = "현재 워프속도"),
	maxSpeed				UMETA(DisplayName = "최대 속도"),
	currentSpeed			UMETA(DisplayName = "현재 속도"),

	maxAcceleration			UMETA(DisplayName = "가속도"),
	minAcceleration			UMETA(DisplayName = "감속도"),
	currentAcceleration		UMETA(DisplayName = "현재 가속팩터"),

	maxRotateRate			UMETA(DisplayName = "최대 회전속도"),
	rotateAcceleraion		UMETA(DisplayName = "회전 가속도"),
	rotateDeceleraion		UMETA(DisplayName = "회전 감속도"),
	currentRotateRate		UMETA(DisplayName = "현재 회전가속팩터"),

	droneBaseStatsMultiple		UMETA(DisplayName = "드론 기초성능 강화"),
	droneDroneDamageMultiple	UMETA(DisplayName = "드론 공격성능 강화"),
	droneDroneRangeMultiple		UMETA(DisplayName = "드론 작전반경 강화"),
	droneDroneSpeedMultiple		UMETA(DisplayName = "드론 기동성능 강화"),
	droneDroneControlMultiple	UMETA(DisplayName = "드론 제어력")
};
UENUM(BlueprintType)
enum class BonusStatType : uint8 {
	CustomBonus				UMETA(DisplayName = "커스텀"),

	BonusMaxShield			UMETA(DisplayName = "최대 쉴드"),
	BonusRechargeShield		UMETA(DisplayName = "쉴드 재생"),
	BonusDefShield			UMETA(DisplayName = "쉴드 방어"),

	BonusMaxArmor			UMETA(DisplayName = "최대 장갑"),
	BonusRepaireArmor		UMETA(DisplayName = "장갑 수리"),
	BonusDefArmor			UMETA(DisplayName = "장갑 방어"),

	BonusMaxHull			UMETA(DisplayName = "최대 선체"),
	BonusRepaireHull		UMETA(DisplayName = "선체 수리"),
	BonusDefHull			UMETA(DisplayName = "선체 방어"),

	BonusMaxPower			UMETA(DisplayName = "최대 전력"),
	BonusRechargePower		UMETA(DisplayName = "전력 생산"),

	BonusMaxRadarRange				UMETA(DisplayName = "탐지 거리"),
	BonusMaxCPUPerfomance			UMETA(DisplayName = "연산력"),
	BonusMaxPowerGridPerfomance		UMETA(DisplayName = "전력망"),
	BonusMaxCargoSize				UMETA(DisplayName = "카고용적"),
	
	BonusMobilitySpeed				UMETA(DisplayName = "최대속도"),
	BonusMobilityAcceleration		UMETA(DisplayName = "가속성능"),
	BonusMobilityRotation			UMETA(DisplayName = "최대 회전속도"),
	BonusMobilityRotateAcceleration	UMETA(DisplayName = "회전 가속성능"),

	BonusActiveRechargeShield	UMETA(DisplayName = "A 모듈 - 쉴드 재생"),
	BonusActiveDefShield		UMETA(DisplayName = "A 모듈 - 쉴드 방어"),
	BonusActiveRepaireArmor		UMETA(DisplayName = "A 모듈 - 장갑 수리"),
	BonusActiveDefArmor			UMETA(DisplayName = "A 모듈 - 장갑 방어"),
	BonusActiveRepaireHull		UMETA(DisplayName = "A 모듈 - 선체 수리"),
	BonusActiveDefHull			UMETA(DisplayName = "A 모듈 - 선체 방어"),

	BonusActiveSpeed			UMETA(DisplayName = "A 모듈 - 최대 속도"),
	BonusActiveAcceleration		UMETA(DisplayName = "A 모듈 - 가속도"),
	BonusActiveRotation			UMETA(DisplayName = "A 모듈 - 회전 속도"),

	BonusActiveModuleUsagePower		UMETA(DisplayName = "A 모듈 - 전력소모 감소"),
	BonusTargetModuleUsagePower		UMETA(DisplayName = "T 모듈 - 전력소모 감소"),

	BonusBeamDamage				UMETA(DisplayName = "빔 - 타격력"),
	BonusBeamPower				UMETA(DisplayName = "빔 - 전력소모 감소"),
	BonusBeamCoolTime			UMETA(DisplayName = "빔 - 방열시간 감소"),
	BonusBeamAccuracy			UMETA(DisplayName = "빔 - 명중률 강화"),
	BonusBeamRange				UMETA(DisplayName = "빔 - 사거리 강화"),

	BonusCannonDamage			UMETA(DisplayName = "캐논 - 타격력"),
	BonusCannonCoolTime			UMETA(DisplayName = "캐논 - 방열시간 감소 - BonusCannonCoolTime"),
	BonusCannonAccuracy			UMETA(DisplayName = "캐논 - 명중률 강화 - BonusCannonAccuracy"),
	BonusCannonLifeTime			UMETA(DisplayName = "캐논 - 포탄 유효시간 강화 - BonusCannonLifeTime"),
	BonusCannonLaunchVelocity	UMETA(DisplayName = "캐논 - 포탄 발사속도 강화 - BonusCannonLaunchVelocity"),

	BonusRailGunDamage			UMETA(DisplayName = "레일건 - 타격력"),
	BonusRailGunPower			UMETA(DisplayName = "레일건 - 전력소모 감소"),
	BonusRailGunCoolTime		UMETA(DisplayName = "레일건 - 방열시간 감소"),
	BonusRailGunAccuracy		UMETA(DisplayName = "레일건 - 명중률 강화"),
	BonusRailGunLifeTime		UMETA(DisplayName = "레일건 - 탄자 유효시간 강화"),
	BonusRailGunLaunchVelocity	UMETA(DisplayName = "레일건 - 탄자 발사속도 강화"),

	BonusMissileDamage			UMETA(DisplayName = "미사일 - 타격력"),
	BonusMissileCoolTime		UMETA(DisplayName = "미사일 - 방열시간 감소"),
	BonusMissileAccuracy		UMETA(DisplayName = "미사일 - 명중률 강화"),
	BonusMissileLifeTime		UMETA(DisplayName = "미사일 - 유효시간 강화"),
	BonusMissileLaunchVelocity	UMETA(DisplayName = "미사일 - 속도 강화"),

	BonusDroneBaseStats		UMETA(DisplayName = "드론 - 기초성능 강화"),
	BonusDroneControl		UMETA(DisplayName = "드론 - 제어력 강화"),
	BonusDroneBay			UMETA(DisplayName = "드론 - 정비시설 확충"),
	BonusDroneDamage		UMETA(DisplayName = "드론 - 공격성능 강화"),
	BonusDroneRange			UMETA(DisplayName = "드론 - 작전반경 강화"),
	BonusDroneSpeed			UMETA(DisplayName = "드론 - 기동성능 강화"),

	BonusMiningAmount		UMETA(DisplayName = "마이닝 - 채광효율 강화"),
	BonusMiningPower		UMETA(DisplayName = "마이닝 - 채광성능 강화"),
	BonusMiningCoolTime		UMETA(DisplayName = "마이닝 - 방열시간 감소"),
	BonusMiningRange		UMETA(DisplayName = "마이닝 - 사거리 강화"),

	BonusReprocessingSlot		UMETA(DisplayName = "재처리 - 유효슬롯 증가"),
	BonusReprocessingTime		UMETA(DisplayName = "재처리 - 시간 효율 증가"),
	BonusReprocessingAmount		UMETA(DisplayName = "재처리 - 처리 효율 증가")
};
UENUM(BlueprintType)
enum class SkillGroup : uint8 {
	TypeOfShipControl			UMETA(DisplayName = "함선 제어"),
	TypeOfShield			UMETA(DisplayName = "쉴드"),
	TypeOfSArmor			UMETA(DisplayName = "장갑"),
	TypeOfHull				UMETA(DisplayName = "선체"),
	TypeOfPower				UMETA(DisplayName = "동력"),
	TypeOfShipEnhance		UMETA(DisplayName = "함선 강화"),
	TypeOfElectronics		UMETA(DisplayName = "전자전"),
	TypeOfMovement			UMETA(DisplayName = "기동"),
	TypeOfWeaponBeam		UMETA(DisplayName = "무기 - 빔"),
	TypeOfWeaponCannon		UMETA(DisplayName = "무기 - 캐논"),
	TypeOfWeaponRailGun		UMETA(DisplayName = "무기 - 레일건"),
	TypeOfWeaponMissile		UMETA(DisplayName = "무기 - 미사일"),
	TypeOfDrone				UMETA(DisplayName = "드론 제어"),
	TypeOfIndustry			UMETA(DisplayName = "산업"),
	TypeOfCommunity			UMETA(DisplayName = "커뮤니케이션")
};
UENUM(BlueprintType)
enum class QuestPlotState : uint8 {
	NotStart				UMETA(DisplayName = "시작하지 않음"),
	Progress				UMETA(DisplayName = "진행 중"),
	ConditionSatisfying		UMETA(DisplayName = "조건부 완료"),
	Finished				UMETA(DisplayName = "진행 완료")
};
UENUM(BlueprintType)
/*	int형 조건 : 0U, 4U, 5U
*	Item형 조건 : 2U, 3U
*	Skill형 조건 : 1U
*	문자열형 조건 : 6U
*/
enum class QuestConditionType : uint8 {
	EnemyDestroy			UMETA(DisplayName = "Enemy Destroy"),
	HavingSkill				UMETA(DisplayName = "Having Skill"),
	HavingItem				UMETA(DisplayName = "Having Item"),
	HavingItemAndGiving		UMETA(DisplayName = "Having Item And Giving"),
	HavingCredit				UMETA(DisplayName = "Having Credit"),
	HavingCreditAndGiving		UMETA(DisplayName = "Having Credit And Giving"),
	MoveToSector			UMETA(DisplayName = "Move To Sector")
};

UENUM(BlueprintType)
enum class ItemType : uint8 {
	Etc					UMETA(DisplayName = "기타"),
	TargetModule		UMETA(DisplayName = "T 타입 모듈"),
	ActiveModule		UMETA(DisplayName = "A 타입 모듈"),
	PassiveModule		UMETA(DisplayName = "P 타입 모듈"),
	SystemModule		UMETA(DisplayName = "S 타입 모듈"),

	Ore					UMETA(DisplayName = "광물"),
	Mineral				UMETA(DisplayName = "미네랄"),
	IndustryParts		UMETA(DisplayName = "산업용품"),
	ConstructPart		UMETA(DisplayName = "건설자재"),
	Blueprint			UMETA(DisplayName = "청사진"),
	Trade				UMETA(DisplayName = "무역품"),

	Ammo_Shell			UMETA(DisplayName = "포탄"),
	Ammo_Sabot			UMETA(DisplayName = "탄자"),
	Ammo_Missile		UMETA(DisplayName = "미사일"),
	Quest				UMETA(DisplayName = "퀘스트 아이템")
};
UENUM(BlueprintType)
enum class ProjectileType : uint8 {
	Shell				UMETA(DisplayName = "포탄"),
	Sabot				UMETA(DisplayName = "탄자"),
	Missile				UMETA(DisplayName = "미사일"),
};
UENUM(BlueprintType)
enum class ModuleType : uint8 {
	NotModule			UMETA(DisplayName = "해당 사항 없음"),

	Beam				UMETA(DisplayName = "빔"),
	Cannon				UMETA(DisplayName = "캐논"),
	Railgun				UMETA(DisplayName = "레일건"),
	MissileLauncher		UMETA(DisplayName = "미사일 런쳐"),

	MinerBeam			UMETA(DisplayName = "채광 광선"),
	TractorBeam			UMETA(DisplayName = "견인 광선"),

	ShieldRegenLaser	UMETA(DisplayName = "쉴드 재생광선"),
	ArmorRepairLaser	UMETA(DisplayName = "장갑 재생광선"),
	HullRepairLaser		UMETA(DisplayName = "선체 재생광선"),
	
	ShieldGenerator		UMETA(DisplayName = "쉴드 생성 모듈"),
	ShieldEnhancer		UMETA(DisplayName = "쉴드 강화 모듈"),
	ArmorRepairer		UMETA(DisplayName = "장갑 수리 모듈"),
	ArmorEnhancer		UMETA(DisplayName = "장갑 강화 모듈"),
	HullRepairer		UMETA(DisplayName = "선체 강화 모듈"),
	HullEnhancer		UMETA(DisplayName = "선체 강화 모듈"),

	EngineController	UMETA(DisplayName = "엔진 제어 모듈"),
	Accelerator			UMETA(DisplayName = "가속 모듈"),
	SteeringController	UMETA(DisplayName = "조향 제어 모듈"),

	PassiveModule		UMETA(DisplayName = "보조 모듈"),
	SystemModule		UMETA(DisplayName = "시스템 모듈")
};
UENUM(BlueprintType)
enum class ModuleSize : uint8 {
	NotSized			UMETA(DisplayName = "해당 사항 없음"),

	Small				UMETA(DisplayName = "소형"),
	Medium				UMETA(DisplayName = "중형"),
	Large				UMETA(DisplayName = "대형"),
	XLarge				UMETA(DisplayName = "확장형"),
	Capital				UMETA(DisplayName = "초대형")
};
UENUM(BlueprintType)
enum class ModuleState : uint8 {
	NotActivate			UMETA(DisplayName = "비활성"),
	Activate			UMETA(DisplayName = "활성"),
	Overload			UMETA(DisplayName = "과부하"),
	ReloadAmmo			UMETA(DisplayName = "재장전")
};

UENUM(BlueprintType)
enum class Region : uint8 {
	UnknownRegionArea_Unclassified	UMETA(DisplayName = "Unknown Region Area Unclassified"),
	Aldir							UMETA(DisplayName = "Aldir"),
	Debrak							UMETA(DisplayName = "Debrak"),
	Cannide							UMETA(DisplayName = "Cannide"),
	Centaur							UMETA(DisplayName = "Centaur"),
	Tashima							UMETA(DisplayName = "Tashima"),

	ElgishShima						UMETA(DisplayName = "Elgish Shima"),
	Hansmit							UMETA(DisplayName = "Hansmit"),
	DomainOfWorld					UMETA(DisplayName = "Domain Of World"),
	CenterOfWorld					UMETA(DisplayName = "Center Of World"),

	UnknownRegionArea_A				UMETA(DisplayName = "Unknown Region Area A"),
	UnknownRegionArea_B				UMETA(DisplayName = "Unknown Region Area B"),
	UnknownRegionArea_C				UMETA(DisplayName = "Unknown Region Area C")
};
UENUM(BlueprintType)
enum class SectorType : uint8 {
	NonInfo				UMETA(DisplayName = "정보 없음"),
	Outer				UMETA(DisplayName = "외각"),
	Border				UMETA(DisplayName = "경계"),
	Normal				UMETA(DisplayName = "일반"),
	Central				UMETA(DisplayName = "중심지"),
	Capital				UMETA(DisplayName = "수도")
};
#pragma endregion

#pragma region Game Play State
UENUM(BlueprintType)
enum class InteractionType : uint8 {
	Info				UMETA(DisplayName = "정보 확인"),
	Communication		UMETA(DisplayName = "통신 시도"),
	DockRequest			UMETA(DisplayName = "도킹 요청"),
	Approach			UMETA(DisplayName = "접근"),
	Attack				UMETA(DisplayName = "공격"),
	Jump				UMETA(DisplayName = "점프"),
	Warp				UMETA(DisplayName = "워프"),
	Collect				UMETA(DisplayName = "채광"),
	Repair				UMETA(DisplayName = "수리"),
	GetCargo			UMETA(DisplayName = "카고 획득")
};
UENUM(BlueprintType)
enum class CargoSortType : uint8 {
	NoOption			UMETA(DisplayName = "정렬 없음"),
	ItemTypeASE			UMETA(DisplayName = "아이템 타입 - 오름차순"),
	ItemTypeDESC		UMETA(DisplayName = "아이템 타입 - 내림차순"),
	ModuleTypeASE		UMETA(DisplayName = "모듈 타입 - 오름차순"),
	ModuleTypeDESC		UMETA(DisplayName = "모듈 타입 - 내림차순"),
	ModuleSizeASE		UMETA(DisplayName = "모듈 크기 - 오름차순"),
	ModuleSizeDESC		UMETA(DisplayName = "모듈 크기 - 내림차순"),
	CargoValueSizeASE	UMETA(DisplayName = "카고 용적 - 오름차순"),
	CargoValueSizeDESC	UMETA(DisplayName = "카고 용적 - 내림차순")
};
UENUM(BlueprintType)
enum class SaveState : uint8 {
	NewGameCreate			UMETA(DisplayName = "새 게임 생성"),
	UserRequest			UMETA(DisplayName = "유저의 요청에 의한 저장"),
	BeforeWarp			UMETA(DisplayName = "워프 전 자동 저장"),
};

UENUM(BlueprintType)
enum class UpdateUIType : uint8 {
	Profile_Info			UMETA(DisplayName = "프로파일 - 유저"),
	Profile_Ship			UMETA(DisplayName = "프로파일 - 함선"),
	Profile_Cargo			UMETA(DisplayName = "프로파일 - 화물"),
	Profile_RelationShip	UMETA(DisplayName = "프로파일 - 관계도"),
	Profile_Quest			UMETA(DisplayName = "프로파일 - 임무"),
	Profile_Asset			UMETA(DisplayName = "프로파일 - 자산"),

	Dock_Main				UMETA(DisplayName = "도킹 - 메인"),
	Dock_Cargo				UMETA(DisplayName = "도킹 - 화물"),
	Dock_Trade				UMETA(DisplayName = "도킹 - 무역"),
	Dock_Industry			UMETA(DisplayName = "도킹 - 산업"),
	Dock_Quest				UMETA(DisplayName = "도킹 - 임무"),
	Dock_StationManagement	UMETA(DisplayName = "도킹 - 스테이션 관리"),
	Dock_SectorManagement	UMETA(DisplayName = "도킹 - 섹터 관리")
};
UENUM(BlueprintType)
enum class MessageLogType : uint8 {
	Info			UMETA(DisplayName = "로그 메세지 유형 - 정보"),
	Warning			UMETA(DisplayName = "로그 메세지 유형 - 경고"),
	Error			UMETA(DisplayName = "로그 메세지 유형 - 에러"),
	Debug			UMETA(DisplayName = "로그 메세지 유형 - 디버깅")
};
#pragma endregion

#pragma region 임시 디버깅용...
UENUM(BlueprintType)
enum class CheatType : uint8 {
	AddItem				UMETA(DisplayName = "아이템 추가"),
	RemoveItem			UMETA(DisplayName = "아이템 제거"),
	AddCredit			UMETA(DisplayName = "크레딧 추가"),
	RemoveCredit		UMETA(DisplayName = "크레딧 제거"),
	AddSkill			UMETA(DisplayName = "스킬 추가"),
	RemoveSkill			UMETA(DisplayName = "스킬 제거"),
	GetSkillList		UMETA(DisplayName = "스킬 리스트 출력"),
	WarpTo				UMETA(DisplayName = "위치로 워프"),
	JumpTo				UMETA(DisplayName = "섹터로 점프"),
	BGMVolume			UMETA(DisplayName = "BGM 볼륨 제어"),
	SfxVolume			UMETA(DisplayName = "Sfx 볼륨 제어")
};
#pragma endregion