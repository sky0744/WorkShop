// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Beam.h"


// Sets default values
ABeam::ABeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	beamParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleBeam"));
	RootComponent = beamParticle;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystem(TEXT("ParticleSystem'/Game/Resource/Particle/Beam/ParticleBeam.ParticleBeam'"));
	if (particleSystem.Succeeded())
		beamParticle->SetTemplate(particleSystem.Object);

	beamShotAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundBeamShot"));
	beamHitAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundBeamHit"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0;

	isHited = false;
}

// Called when the game starts or when spawned
void ABeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABeam::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!IsValid(target) || !target->IsA(ASpaceObject::StaticClass()))
		return;

	startLocation = beamOwner->GetActorLocation() + beamOwner->GetActorRotation().RotateVector(startLocation);
	DrawDebugPoint(GetWorld(), startLocation, 6, FColor(255, 255, 255), false, DeltaTime);
	//거리 판단상 사거리 이내
	if (beamRange > USafeENGINE::CheckDistanceConsiderSize(beamOwner, target)) {
		launchedFaction = beamOwner->GetFaction();
		resultLocation = target->GetActorLocation();
		isHited = true;
	//빔이 블럭되지 않고 최대거리까지 진행
	} else {
		resultLocation = target->GetActorLocation() - beamOwner->GetActorLocation();
		resultLocation.Normalize();
		resultLocation = GetActorLocation() + resultLocation * beamRange;
		isHited = false;
	}
	DrawDebugPoint(GetWorld(), resultLocation, 6, FColor(255, 255, 255), false, DeltaTime);
	DrawDebugLine(GetWorld(), startLocation, resultLocation, FColor(255, 255, 255), false, DeltaTime, 0, 5.0f);

	FItem _collectedOre;
	if (isHited) {
		switch (beamType) {
		case ModuleType::MinerLaser:
			if (!IsValid(target) || !target->IsA(AResource::StaticClass()))
				return;

			_collectedOre = Cast<AResource>(target)->CollectResource(beamDamage);
			if (beamOwner->GetInstigatorController()->GetClass() == AUserController::StaticClass())
				Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->AddPlayerCargo(_collectedOre);
			break;
		case ModuleType::TractorBeam:
			if (!IsValid(target) || !target->IsA(ACargoContainer::StaticClass()))
				return;

			resultLocation = (target->GetActorLocation() - beamOwner->GetActorLocation());
			resultLocation.Normalize();
			target->AddActorWorldOffset(resultLocation * beamDamage);
			break;
		default:
			break;
		}
	}
}

//발사 전에 빔의 속성을 지정.
void ABeam::SetBeamProperty(ASpaceObject* launchActor, ASpaceObject* targetActor, float setedrange, ModuleType setedbeamType, float setedDamage, FVector locOffset, float aliveTime) {

	if (!IsValid(targetActor) || !targetActor->IsA(ASpaceObject::StaticClass()))
		return;
	beamType = setedbeamType;

	//beamParticle->SetBeamTargetPoint(0, _beamHitresult.Location, 0);
	switch (beamType) {
	case ModuleType::Beam:
		resultLocation = targetActor->GetActorLocation() - GetActorLocation();
		resultLocation.Normalize();
		DrawDebugPoint(GetWorld(), GetActorLocation(), 6, FColor(255, 255, 255), false, aliveTime);

		projectileShotSound =
		//거리 판단상 사거리 이내
		if (setedrange > USafeENGINE::CheckDistanceConsiderSize(launchActor, targetActor)) {
			launchedFaction = launchActor->GetFaction();
			UGameplayStatics::ApplyPointDamage(targetActor, setedDamage, resultLocation, FHitResult(), nullptr, this, UDamageType::StaticClass());
			resultLocation = targetActor->GetActorLocation();
			beamHitSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *currentSectorInfo->PlayerableBGM[_bgmIndex].ToString()));

			//빔이 블럭되지 않고 최대거리까지 진행
		} else 
			resultLocation = GetActorLocation() + resultLocation * setedrange;
		DrawDebugPoint(GetWorld(), resultLocation, 6, FColor(255, 255, 255), false, aliveTime);
		DrawDebugLine(GetWorld(), GetActorLocation(), resultLocation, FColor(255, 255, 255), false, aliveTime, 0, 5.0f);
		break;
	case ModuleType::MinerLaser:
		if (targetActor->IsA(AResource::StaticClass())) {
			beamOwner = launchActor;
			target = targetActor;
			beamRange = setedrange;
			beamDamage = setedDamage;
			startLocation = locOffset;
			PrimaryActorTick.TickInterval = 0.333333f;
		}
		break;
	case ModuleType::TractorBeam:
		if (targetActor->IsA(ACargoContainer::StaticClass())) {
			beamOwner = launchActor;
			target = targetActor;
			beamRange = setedrange;
			beamDamage = FMath::Clamp(setedDamage, -250.0f, 250.0f);
			startLocation = locOffset;
			PrimaryActorTick.TickInterval = 0.0f;
		}
		break;
	default:
		break;
	}
	this->SetLifeSpan(aliveTime);
}

Faction ABeam::GetLaunchingFaction() const{
	return launchedFaction;
}