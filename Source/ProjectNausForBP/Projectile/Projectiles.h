// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Projectiles.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API AProjectiles : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectiles();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
		/** 엑터와 충돌시 호출
		*	@param HitResult - 충돌 결과
		*/
		void OnCollisionActor(const FHitResult& hitResult);

	UFUNCTION()
		/** 프로젝타일 속성을 세팅
		*	@param ammoID - ID
		*	@param launchActor - 발사한 엑터
		*	@param damageMultiple - 발사한 엑터의 데미지 보너스 수치
		*	@param maxSpeedMultiple - 발사한 엑터의 발사속도 보너스 수치
		*	@param lifetimeMultiple - 프로젝타일의 유효시간
		*	@param targetObject - 목표 설정. 미사일 타입의 프로젝타일에 적용
		*/
		void SetProjectileProperty(int ammoID, ASpaceObject* launchActor, float damageMultiple, float maxSpeedMultiple,
			float lifetimeMultiple, ASpaceObject* targetObject = nullptr);

#pragma region Projectile Property
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Projectile Data")
		UStaticMeshComponent* projectileMesh;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Projectile Data")
		UProjectileMovementComponent* projectileMovement;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Projectile Data")
		UParticleSystemComponent* projectileParticle;

	UPROPERTY()
		ASpaceObject* projectileOwner;
	UPROPERTY()
		ASpaceObject* target;
	bool isHoming;

	float setedDamage;
	float setedMaxSpeed;
	float currentSpeed;
#pragma endregion
};
