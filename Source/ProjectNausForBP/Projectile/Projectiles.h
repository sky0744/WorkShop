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
		/** ���Ϳ� �浹�� ȣ��
		*	@param HitResult - �浹 ���
		*/
		void OnCollisionActor(const FHitResult& hitResult);

	UFUNCTION()
		/** ������Ÿ�� �Ӽ��� ����
		*	@param ammoID - ID
		*	@param launchActor - �߻��� ����
		*	@param damageMultiple - �߻��� ������ ������ ���ʽ� ��ġ
		*	@param maxSpeedMultiple - �߻��� ������ �߻�ӵ� ���ʽ� ��ġ
		*	@param lifetimeMultiple - ������Ÿ���� ��ȿ�ð�
		*	@param targetObject - ��ǥ ����. �̻��� Ÿ���� ������Ÿ�Ͽ� ����
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
