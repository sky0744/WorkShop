// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PaperFlipbook.h"
#include "PaperSpriteComponent.h"
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
		*	@param OverlappedComp - 충돌된 컴포넌트
		*	@param OtherActor - 충돌한 엑터
		*	@param OtherComp - 충돌한 엑터의 컴포넌트
		*	@param OtherBodyIndex - ?
		*	@param bFromSweep - 스윕 여부
		*	@param SweepResult - 충돌 물리 결과
		*/
		void OnCollisionActor(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Projectile Data")
		Faction GetLaunchingFaction() const;
#pragma region Projectile Property
private:
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		USphereComponent* projectileHitSensor;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		UPaperFlipbook* projectileFlipBook;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		UPaperSpriteComponent* projectileSprite;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		UParticleSystemComponent* projectileParticle;

	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		UAudioComponent* projectileShotAudio;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		UAudioComponent* projectileMovementAudio;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		UAudioComponent* projectileHitAudio;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		USoundCue* projectileShotSound;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		USoundCue* projectileMovementSound;
	UPROPERTY(VisibleAnyWhere, Category = "Projectile Data")
		USoundCue* projectileHitSound;
	
	UPROPERTY()
		ASpaceObject* projectileOwner;
	UPROPERTY()
		ASpaceObject* target;
	float projectileYaw;

	Faction launchedFaction;
	bool isHoming;
	float activateTime;
	float setedDamage;
	float setedVelocity;
	float currentVelocity;

	float coldVelocityFactor;
	float coldAcceleration;
	float coldDelayTime;
#pragma endregion
};
