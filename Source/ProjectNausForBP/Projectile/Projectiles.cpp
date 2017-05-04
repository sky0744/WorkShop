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
	if (!isHoming)
		AddActorWorldOffset(GetActorForwardVector() * setedVelocity * DeltaTime, false);

	//유도탄이며 발사 2초 이내 -> 현재 방향 및 발사 속도의 일정 비율을 유지하며 진행
	else if(activateTime < 2.0f)
		AddActorWorldOffset(GetActorForwardVector() * setedVelocity * DeltaTime * 0.15f, false);
	
	//유도탄이며 발사 2초 이상, 타겟이 유효 -> 유도 및 발사 속도를 유지하며 진행
	else if(USafeENGINE::IsValid(target)){
		SetActorRotation(FMath::Lerp(GetActorRotation(), (target->GetActorLocation() - GetActorLocation()).Rotation(), 0.1f));
		AddActorWorldOffset(GetActorForwardVector() * setedVelocity * DeltaTime, false);
	} else
		Destroy();
}

void AProjectiles::OnCollisionActor(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (USafeENGINE::IsValid(OtherActor) &&  OtherActor->IsA(ASpaceObject::StaticClass()) && OtherActor != projectileOwner) {
		UGameplayStatics::ApplyPointDamage(OtherActor, setedDamage, FVector(1.0f, 0.0f, 0.0f), SweepResult, nullptr, this, UDamageType::StaticClass());
		Destroy();
	}
}

void AProjectiles::SetProjectileProperty(int ammoID, ASpaceObject* launchActor, float damageMultiple,
	float maxSpeedMultiple, float lifetimeMultiple, ASpaceObject* targetObject) {
	if (!USafeENGINE::IsValid(targetObject)) {
		Destroy();
		return;
	}

	USafeENGINE* _tempInstance = Cast<USafeENGINE>(GetGameInstance());
	FItemData _tempItemData = _tempInstance->GetItemData(ammoID);
	FProjectileData _tempProjectileData = _tempInstance->GetProjectileData(_tempItemData.ProjectileID);

	UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *_tempProjectileData.MeshPath.ToString()));
	if(newMesh)
		projectileMesh->SetStaticMesh(newMesh);
	projectileHitSensor->SetSphereRadius(_tempProjectileData.ExplosionSensorRange);

	launchedFaction = launchActor->GetFaction();
	setedDamage = _tempItemData.Damage * damageMultiple;
	projectileOwner = launchActor;
	setedVelocity = _tempItemData.LaunchSpeed * maxSpeedMultiple;

	if (_tempItemData.Type == ItemType::Ammo_Missile) {
		isHoming = true;
		target = targetObject;
	} else 
		isHoming = false;

	projectileMesh->IgnoreActorWhenMoving(projectileOwner, true);
	SetLifeSpan(_tempItemData.LifeTime * lifetimeMultiple);
}

Faction AProjectiles::GetLaunchingFaction() const{
	return launchedFaction;
}
