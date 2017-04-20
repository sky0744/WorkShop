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

	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0;
}

// Called when the game starts or when spawned
void ABeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABeam::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

//발사 전에 빔의 속성을 지정.
void ABeam::SetBeamProperty(ASpaceObject* launchActor, FVector targetedLocation, bool isWeapon, float setedDamage, float aliveTime) {

	FCollisionObjectQueryParams _traceObjectParams;
	FCollisionQueryParams _traceParams = FCollisionQueryParams(FName("PathFind"), true, this);
	FHitResult _beamHitresult;

	this->GetWorld()->LineTraceSingleByObjectType(_beamHitresult
		, this->GetActorLocation()
		, targetedLocation
		, _traceObjectParams
		, _traceParams);

	DrawDebugPoint(GetWorld(), GetActorLocation(), 20, FColor(255, 255, 255), false, aliveTime);
	//빔이 블럭되었을 때
	if (_beamHitresult.bBlockingHit) {
		DrawDebugPoint(GetWorld(), _beamHitresult.Location, 20, FColor(255, 255, 255), false, aliveTime);
		DrawDebugLine(GetWorld(), GetActorLocation(), _beamHitresult.Location, FColor(255, 255, 255), false, 0.1f);
		//beamParticle->SetBeamTargetPoint(0, _beamHitresult.Location, 0);

		if (_beamHitresult.Actor->IsA(ASpaceObject::StaticClass())) {
			if (isWeapon)
				UGameplayStatics::ApplyPointDamage(_beamHitresult.Actor.Get(), setedDamage, FVector(1.0f, 0.0f, 0.0f),
					_beamHitresult, launchActor->GetInstigatorController(), launchActor, UDamageType::StaticClass());
			else {
				if (_beamHitresult.Actor->IsA(AResource::StaticClass()) == true) {
					FItem _collectedOre = Cast<AResource>(_beamHitresult.Actor.Get())->CollectResource(setedDamage);
					if(launchActor->GetInstigatorController()->GetClass() == AUserController::StaticClass())
						Cast<AUserState>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState)->AddPlayerCargo(_collectedOre);
				}
			}
		}
	}
	//빔이 블럭되지 않고 최대거리까지 진행
	else {
		DrawDebugPoint(GetWorld(), targetedLocation, 20, FColor(255, 255, 255), false, aliveTime);
		DrawDebugLine(GetWorld(), GetActorLocation(), targetedLocation, FColor(255, 255, 255), false, aliveTime, 0, 5.0f);
		//beamParticle->SetBeamTargetPoint(0, targetedLocation, 0);
	}

	this->SetLifeSpan(aliveTime);
}