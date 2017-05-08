// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Projectiles.h"


// Sets default values
AProjectiles::AProjectiles()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = projectileMesh;
	projectileMesh->SetCollisionProfileName(TEXT("Projectile"));
	projectileMesh->SetEnableGravity(false);
	projectileMesh->SetSimulatePhysics(false);
	projectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleBeam"));
	projectileHitSensor = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionSensor"));
	projectileHitSensor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	projectileHitSensor->OnComponentBeginOverlap.AddDynamic(this, &AProjectiles::OnCollisionActor);

	projectileShotAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundProjectileShot"));
	projectileShotAudio->bAutoActivate = false;
	projectileMovementAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundProjectileMovement"));
	projectileMovementAudio->bAutoActivate = false;
	projectileHitAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundProjectileHit"));
	projectileHitAudio->bAutoActivate = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0;
}

// Called when the game starts or when spawned
void AProjectiles::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectiles::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	activateTime += DeltaTime;
	//유도탄이 아닌 경우 -> 현재 방향 및 발사 속도를 유지하며 진행
	if (!isHoming) {
		AddActorWorldOffset(GetActorForwardVector() * setedVelocity * DeltaTime, false);
		projectileMovementAudio->VolumeMultiplier = 1.0f;
	}

	//유도탄인 경우
	else {
		if (activateTime < 2.0f)
			currentVelocity = setedVelocity * coldVelocityFactor;
		//유도탄이며 발사 2초 이상, 타겟이 유효 -> 유도 및 발사 속도를 유지하며 진행
		else if (IsValid(target)) {
			currentVelocity = FMath::Clamp(currentVelocity + coldAcceleration * DeltaTime, _define_ProjectileVelocityMIN, setedVelocity);
			SetActorRotation(FMath::Lerp(GetActorRotation(), (target->GetActorLocation() - GetActorLocation()).Rotation(), 0.1f));
		}
		projectileMovementSound->PitchMultiplier = currentVelocity / setedVelocity;
		AddActorWorldOffset(GetActorForwardVector() * currentVelocity * DeltaTime, false);
	}
}

void AProjectiles::OnCollisionActor(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (IsValid(OtherActor) &&  OtherActor->IsA(ASpaceObject::StaticClass()) && OtherActor != projectileOwner) {
		UGameplayStatics::ApplyPointDamage(OtherActor, setedDamage, FVector::ForwardVector, SweepResult, nullptr, this, UDamageType::StaticClass());
		if (projectileMovementAudio)
			projectileMovementAudio->Stop();
		if (projectileHitSound)
			projectileHitAudio->Play();
		Destroy();
	}
}

void AProjectiles::SetProjectileProperty(int ammoID, ASpaceObject* launchActor, float damageMultiple,
	float maxSpeedMultiple, float lifetimeMultiple, ASpaceObject* targetObject) {
	if (!IsValid(targetObject)) {
		Destroy();
		return;
	}

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempItemData = _tempInstance->GetItemData(ammoID);
	FProjectileData _tempProjectileData = _tempInstance->GetProjectileData(_tempItemData.ProjectileID);

	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempProjectileData.MeshPath.ToString()));
	if (newMesh)
		projectileMesh->SetStaticMesh(newMesh);
	
	launchedFaction = launchActor->GetFaction();
	setedDamage = _tempItemData.Damage * damageMultiple;
	projectileOwner = launchActor;
	setedVelocity = FMath::Clamp(_tempItemData.LaunchSpeed * maxSpeedMultiple, _define_ProjectileVelocityMIN, _define_ProjectileVelocityMAX);

	projectileHitSensor->SetSphereRadius(FMath::Clamp(_tempProjectileData.ExplosionSensorRange, _define_ProjectileExplosiveRadiusMIN, _define_ProjectileExplosiveRadiusMAX));
	coldVelocityFactor = FMath::Clamp(_tempProjectileData.ColdLaunchVelocityFactor, _define_ProjectileColdLaunchVelocityMIN, _define_ProjectileColdLaunchVelocityMAX);
	coldAcceleration = FMath::Clamp(_tempProjectileData.ColdLaunchAcceleration, _define_ProjectileColdLaunchAccelMIN, _define_ProjectileColdLaunchAccelMAX);
	coldDelayTime = FMath::Clamp(_tempProjectileData.ColdLaunchDelayTime, _define_ProjectileColdLaunchDelayMIN, _define_ProjectileColdLaunchDelayMAX);
	 
	projectileShotSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *_tempProjectileData.SfxShotPath.ToString()));
	projectileMovementSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *_tempProjectileData.SfxMovementPath.ToString()));
	projectileHitSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *_tempProjectileData.SfxHitPath.ToString()));

	if (projectileShotSound) {
		projectileShotAudio->SetSound(projectileShotSound);
		projectileShotAudio->Play();
	}
	if (projectileMovementSound) {
		projectileMovementAudio->SetSound(projectileMovementSound);
		projectileMovementAudio->VolumeMultiplier = 0.0f;
		projectileMovementAudio->Play();
	}
	if (projectileHitSound)
		projectileHitAudio->SetSound(projectileHitSound);

	if (_tempItemData.Type == ItemType::Ammo_Missile) {
		isHoming = true;
		target = targetObject;
	} else 
		isHoming = false;

	projectileMesh->IgnoreActorWhenMoving(projectileOwner, true);
	SetLifeSpan(FMath::Clamp(_tempItemData.LifeTime * lifetimeMultiple, _define_ProjectileLifeTimeMIN, _define_ProjectileLifeTimeMAX));
}

Faction AProjectiles::GetLaunchingFaction() const{
	return launchedFaction;
}
