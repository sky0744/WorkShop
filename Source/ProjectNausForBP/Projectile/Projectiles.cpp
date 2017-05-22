﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectNausForBP.h"
#include "Projectiles.h"


// Sets default values
AProjectiles::AProjectiles() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	projectileHitSensor = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileSensor"));
	projectileHitSensor->OnComponentBeginOverlap.AddDynamic(this, &AProjectiles::OnCollisionActor);
	projectileHitSensor->SetEnableGravity(false);
	projectileHitSensor->SetCollisionProfileName(TEXT("SpaceObject"));
	projectileHitSensor->BodyInstance.DOFMode = EDOFMode::XYPlane;
	projectileHitSensor->Mobility = EComponentMobility::Movable;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	RootComponent = projectileHitSensor;

	projectileSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("projectileSprte"));
	projectileSprite->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	projectileSprite->bAbsoluteRotation = true;
	projectileSprite->SetWorldRotation(FRotator(0.0f, 90.0f, -90.0f));
	projectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleBeam"));

	projectileShotAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundProjectileShot"));
	projectileShotAudio->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	projectileShotAudio->bAutoActivate = false;
	projectileMovementAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundProjectileMovement"));
	projectileShotAudio->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	projectileMovementAudio->bAutoActivate = false;
	projectileHitAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundProjectileHit"));
	projectileShotAudio->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	projectileHitAudio->bAutoActivate = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bTickEvenWhenPaused = false;
	PrimaryActorTick.TickInterval = 0.0;
	activateTime = 0.0f;
}

// Called when the game starts or when spawned
void AProjectiles::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AProjectiles::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsValid(projectileFlipBook) && projectileFlipBook->GetNumFrames() > 1) {
		projectileYaw = GetActorRotation().Yaw;
		projectileYaw += (360.0f / projectileFlipBook->GetNumFrames()) * 0.5f;
		if (projectileYaw < 0.0f)
			projectileYaw += 360.0f;
		projectileYaw /= (360.0f / projectileFlipBook->GetNumFrames());
		//objectFlipBook->SetNewTime(objectYaw);
		projectileSprite->SetSprite(projectileFlipBook->GetSpriteAtFrame(FMath::TruncToInt(projectileYaw)));
	}
	activateTime += DeltaTime;
	//유도탄이 아닌 경우 -> 현재 방향 및 발사 속도를 유지하며 진행
	if (!isHoming) {
		AddActorWorldOffset(GetActorForwardVector() * setedVelocity * DeltaTime, false);
		if (projectileMovementAudio)
			projectileMovementAudio->VolumeMultiplier = 1.0f;
	}

	//유도탄인 경우
	else {
		if (activateTime < coldDelayTime)
			currentVelocity = setedVelocity * coldVelocityFactor;
		//발사 지연시간 후, 타겟이 유효 -> 유도 및 발사 속도를 최대치까지 증가시키며 진행
		else if (IsValid(target)) {
			currentVelocity = FMath::Clamp(currentVelocity + coldAcceleration * DeltaTime, _define_ProjectileVelocityMIN, setedVelocity);
			SetActorRotation(FMath::Lerp(GetActorRotation(), (target->GetActorLocation() - GetActorLocation()).Rotation(), 0.1f));
		}
		if (projectileMovementAudio)
			projectileMovementAudio->PitchMultiplier = currentVelocity / setedVelocity;
		AddActorWorldOffset(GetActorForwardVector() * currentVelocity * DeltaTime, false);
	}
}

void AProjectiles::OnCollisionActor(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (IsValid(OtherActor) && OtherActor->IsA(ASpaceObject::StaticClass()) && OtherActor != projectileOwner) {
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

	if (_tempProjectileData.FlipSprite) {
		projectileFlipBook = _tempProjectileData.FlipSprite;
		projectileSprite->SetSprite(projectileFlipBook->GetSpriteAtFrame(0));
	}

	launchedFaction = launchActor->GetFaction();
	setedDamage = _tempItemData.Damage * damageMultiple;
	projectileOwner = launchActor;
	setedVelocity = FMath::Clamp(_tempItemData.LaunchSpeed * maxSpeedMultiple, _define_ProjectileVelocityMIN, _define_ProjectileVelocityMAX);

	projectileHitSensor->SetSphereRadius(FMath::Clamp(_tempProjectileData.ExplosionSensorRange, _define_ProjectileExplosiveRadiusMIN, _define_ProjectileExplosiveRadiusMAX));
	coldVelocityFactor = FMath::Clamp(_tempProjectileData.ColdLaunchVelocityFactor, _define_ProjectileColdLaunchVelocityMIN, _define_ProjectileColdLaunchVelocityMAX);
	coldAcceleration = FMath::Clamp(_tempProjectileData.ColdLaunchAcceleration, _define_ProjectileColdLaunchAccelMIN, _define_ProjectileColdLaunchAccelMAX);
	coldDelayTime = FMath::Clamp(_tempProjectileData.ColdLaunchDelayTime, _define_ProjectileColdLaunchDelayMIN, _define_ProjectileColdLaunchDelayMAX);

	if (_tempProjectileData.SfxShotPath.Compare("None") != 0) {
		projectileShotSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *_tempProjectileData.SfxShotPath.ToString()));
		if (projectileShotSound) {
			projectileShotAudio->SetSound(projectileShotSound);
			projectileShotAudio->VolumeMultiplier = 1.0f;
			projectileShotAudio->Play();
		}
	}
	if (_tempProjectileData.SfxMovementPath.Compare("None") != 0) {
		projectileMovementSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *_tempProjectileData.SfxMovementPath.ToString()));
		if (projectileMovementSound) {
			projectileMovementAudio->SetSound(projectileMovementSound);
			projectileMovementAudio->VolumeMultiplier = 0.0f;
			projectileMovementAudio->Play();
		}
	}
	if (_tempProjectileData.SfxHitPath.Compare("None") != 0) {
		projectileHitSound = Cast<USoundCue>(StaticLoadObject(USoundCue::StaticClass(), NULL, *_tempProjectileData.SfxHitPath.ToString()));
		if (projectileHitSound)
			projectileHitAudio->SetSound(projectileHitSound);
	}

	if (_tempItemData.Type == ItemType::Ammo_Missile) {
		isHoming = true;
		target = targetObject;
	} else
		isHoming = false;

	SetLifeSpan(FMath::Clamp(_tempItemData.LifeTime * (1.0f + lifetimeMultiple), _define_ProjectileLifeTimeMIN, _define_ProjectileLifeTimeMAX));
}

Faction AProjectiles::GetLaunchingFaction() const {
	return launchedFaction;
}
