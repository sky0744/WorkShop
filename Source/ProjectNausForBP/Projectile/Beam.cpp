// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Beam.h"


// Sets default values
ABeam::ABeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	beamParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleBeam"));
	RootComponent = beamParticle ;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particleSystem(TEXT("ParticleSystem'/Game/Resource/Particle/Beam/ParticleBeam.ParticleBeam'"));
	if (particleSystem.Succeeded())
		beamParticle->SetTemplate(particleSystem.Object);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0;

	_traceParams = FCollisionQueryParams(FName("Beam"), true, this);
}

// Called when the game starts or when spawned
void ABeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABeam::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!USafeENGINE::IsValid(target) || !target->IsA(ASpaceObject::StaticClass()))
		return;

	resultLocation = target->GetActorLocation() - GetActorLocation();
	resultLocation.Normalize();
	SetActorLocation(beamOwner->GetActorLocation() + resultLocation * beamOwner->GetValue(GetStatType::halfLength));
	resultLocation *= beamRange;

	this->GetWorld()->LineTraceSingleByObjectType(_beamHitresult
		, this->GetActorLocation()
		, this->GetActorLocation() + resultLocation
		, _traceObjectParams
		, _traceParams);

	//beamParticle->SetBeamTargetPoint(0, targetedLocation, 0);
	switch (beamType) {
	case ModuleType::MinerLaser:
		DrawDebugPoint(GetWorld(), GetActorLocation(), 20, FColor(255, 255, 255), false, DeltaTime);
		//빔이 블럭되었을 때
		if (_beamHitresult.bBlockingHit) {
			if (_beamHitresult.Actor->IsA(AResource::StaticClass())) {
				FItem _collectedOre = Cast<AResource>(_beamHitresult.Actor.Get())->CollectResource(beamDamage);
				if (beamOwner->GetInstigatorController()->GetClass() == AUserController::StaticClass())
					Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->AddPlayerCargo(_collectedOre);
			}
			resultLocation = _beamHitresult.ImpactPoint;
		}
		//빔이 블럭되지 않은 경우 최대거리까지 진행
		DrawDebugPoint(GetWorld(), resultLocation, 20, FColor(255, 255, 255), false, DeltaTime);
		DrawDebugLine(GetWorld(), GetActorLocation(), resultLocation, FColor(255, 255, 255), false, DeltaTime, 0, 5.0f);
		break;
	case ModuleType::TractorBeam:
		DrawDebugPoint(GetWorld(), GetActorLocation(), 20, FColor(255, 255, 255), false, DeltaTime);
		//빔이 블럭되었을 때
		if (_beamHitresult.bBlockingHit) {
			if (_beamHitresult.Actor->IsA(ACargoContainer::StaticClass())) {
				resultLocation = (target->GetActorLocation() - beamOwner->GetActorLocation());
				resultLocation.Normalize();
				_beamHitresult.Actor->AddActorWorldOffset(resultLocation * beamDamage);
			}
			resultLocation = _beamHitresult.ImpactPoint;
		}
		DrawDebugPoint(GetWorld(), resultLocation, 20, FColor(255, 255, 255), false, DeltaTime);
		DrawDebugLine(GetWorld(), GetActorLocation(), resultLocation, FColor(255, 255, 255), false, DeltaTime, 0, 5.0f);
		//빔이 블럭되지 않은 경우 최대거리까지 진행
		break;
	default:
		break;
	}
}

//발사 전에 빔의 속성을 지정.
void ABeam::SetBeamProperty(ASpaceObject* launchActor, ASpaceObject* targetActor, float setedrange, ModuleType setedbeamType, float setedDamage, float aliveTime) {

	if (!USafeENGINE::IsValid(targetActor) || !targetActor->IsA(ASpaceObject::StaticClass()))
		return;
	beamType = setedbeamType;

	//beamParticle->SetBeamTargetPoint(0, _beamHitresult.Location, 0);
	switch (beamType) {
	case ModuleType::Beam:
		resultLocation = targetActor->GetActorLocation() - GetActorLocation();
		resultLocation.Normalize();
		resultLocation *= setedrange;
		DrawDebugPoint(GetWorld(), GetActorLocation(), 20, FColor(255, 255, 255), false, aliveTime);

		this->GetWorld()->LineTraceSingleByObjectType(_beamHitresult
			, this->GetActorLocation()
			, this->GetActorLocation() + resultLocation
			, _traceObjectParams
			, _traceParams);
		//빔이 블럭되었을 때
		if (_beamHitresult.bBlockingHit) {
			resultLocation = _beamHitresult.ImpactPoint;
			launchedFaction = launchActor->GetFaction();
			if (_beamHitresult.Actor->IsA(ASpaceObject::StaticClass()))
				UGameplayStatics::ApplyPointDamage(_beamHitresult.Actor.Get(), setedDamage, FVector(1.0f, 0.0f, 0.0f),
					_beamHitresult, nullptr, this, UDamageType::StaticClass());
			//빔이 블럭되지 않고 최대거리까지 진행
			else {
				DrawDebugPoint(GetWorld(), resultLocation, 20, FColor(255, 255, 255), false, aliveTime);
				DrawDebugLine(GetWorld(), GetActorLocation(), resultLocation, FColor(255, 255, 255), false, aliveTime, 0, 5.0f);
				//beamParticle->SetBeamTargetPoint(0, targetedLocation, 0);
			}
		}
		DrawDebugPoint(GetWorld(), _beamHitresult.Location, 20, FColor(255, 255, 255), false, aliveTime);
		DrawDebugLine(GetWorld(), GetActorLocation(), _beamHitresult.Location, FColor(255, 255, 255), false, 0.1f);
		break;
	case ModuleType::MinerLaser:
		if (targetActor->IsA(AResource::StaticClass())) {
			beamOwner = launchActor;
			target = targetActor;
			beamRange = setedrange;
			beamDamage = setedDamage;
			PrimaryActorTick.TickInterval = 0.333333f;
		}
		break;
	case ModuleType::TractorBeam:
		if (targetActor->IsA(ACargoContainer::StaticClass())) {
			beamOwner = launchActor;
			target = targetActor;
			beamRange = setedrange;
			beamDamage = FMath::Clamp(setedDamage, -250.0f, 250.0f);
			PrimaryActorTick.TickInterval = 0.0f;
		}
		break;
	default:
		break;
	}
	this->SetLifeSpan(aliveTime);
}

Faction ABeam::GetLaunchingFaction() {
	return launchedFaction;
}