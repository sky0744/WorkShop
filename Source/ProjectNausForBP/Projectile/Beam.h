// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectNausForBP.h"
#include "Beam.generated.h"

UCLASS()
class PROJECTNAUSFORBP_API ABeam : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeam();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	/** 프로젝타일 속성을 세팅
	*	@param launchActor - 발사한 엑터
	*	@param targetedLocation - 목표 지점
	*	@param isWeapon - 빔의 타입
	*	@param setedDamage - 빔의 강도
	*	@param aliveTime - 빔 이펙트 시간
	*/
	void SetBeamProperty(ASpaceObject* launchActor, FVector targetedLocation, bool isWeapon, float setedDamage, float aliveTime = 1.0f);
#pragma region Beam Property
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Beam Data")
		UParticleSystemComponent* beamParticle;
#pragma endregion
};
