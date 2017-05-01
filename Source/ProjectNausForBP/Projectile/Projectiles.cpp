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
	projectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleBeam"));
	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	projectileMovement->UpdatedComponent = projectileMesh;
	projectileMovement->OnProjectileStop.AddDynamic(this, &AProjectiles::OnCollisionActor);
	projectileMovement->bShouldBounce = true;
	//projectileMovement->Bounciness = 0.0f;

	PrimaryActorTick.bCanEverTick = false;
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
void AProjectiles::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

void AProjectiles::OnCollisionActor(const FHitResult& hitResult) {
	if (hitResult.Actor.Get()->IsA(ASpaceObject::StaticClass())) {
		UGameplayStatics::ApplyPointDamage(hitResult.Actor.Get(), setedDamage, FVector(1.0f, 0.0f, 0.0f), hitResult, nullptr, this, UDamageType::StaticClass());
		Destroy();
	}
}

void AProjectiles::SetProjectileProperty(int ammoID, ASpaceObject* launchActor, float damageMultiple,
	float maxSpeedMultiple, float lifetimeMultiple, ASpaceObject* targetObject) {

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempItemData = _tempInstance->GetItemData(ammoID);
	FProjectileData _tempProjectileData = _tempInstance->GetProjectileData(_tempItemData.ProjectileID);

	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempProjectileData.MeshPath.ToString()));
	if(newMesh)
		projectileMesh->SetStaticMesh(newMesh);

	launchedFaction = launchActor->GetFaction();
	setedDamage = _tempItemData.Damage * damageMultiple;
	
	target = targetObject;
	if (_tempItemData.Type == ItemType::Ammo_Missile && targetObject) {
		projectileMovement->bIsHomingProjectile = true;
		projectileMovement->HomingTargetComponent = targetObject->GetRootComponent();
		projectileMovement->HomingAccelerationMagnitude = 1500.0f;
		projectileMovement->MaxSpeed = _tempItemData.LaunchSpeed * maxSpeedMultiple;
		projectileMovement->InitialSpeed = 10.0f * maxSpeedMultiple;
	}
	else 
		projectileMovement->Velocity = _tempItemData.LaunchSpeed * maxSpeedMultiple * GetActorForwardVector();

	this->SetLifeSpan(_tempItemData.LifeTime * (1.0f + lifetimeMultiple));
}

Faction AProjectiles::GetLaunchingFaction() const{
	return launchedFaction;
}