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
	*	@param targetActor - 목표 대상
	*	@param range - 최대 거리
	*	@param beamType - 빔의 타입(빔계열 이외 - 무효)
	*	@param setedDamage - 빔의 강도
	*	@param aliveTime - 빔 이펙트 시간
	*/
	void SetBeamProperty(ASpaceObject* launchActor, ASpaceObject* targetActor, float setedrange, ModuleType setedbeamType, float setedDamage, FVector locOffset, float aliveTime = 1.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintPure, Category = "Beam Data")
		Faction GetLaunchingFaction() const;
#pragma region Beam Property
private:
	UPROPERTY(VisibleAnyWhere, Category = "Beam Data")
		UParticleSystemComponent* beamParticle;
	UPROPERTY(VisibleAnyWhere, Category = "Beam Data")
		UAudioComponent* beamShotAudio;
	UPROPERTY(VisibleAnyWhere, Category = "Beam Data")
		UAudioComponent* beamHitAudio;
	UPROPERTY(VisibleAnyWhere, Category = "Beam Data")
		USoundCue* beamShotSound;
	UPROPERTY(VisibleAnyWhere, Category = "Beam Data")
		USoundCue* beamHitSound;
	ModuleType beamType;

	UPROPERTY()
		ASpaceObject* beamOwner;
	UPROPERTY()
		ASpaceObject* target;

	Faction launchedFaction;
	float beamRange;
	float beamDamage;
	FVector startLocation;
	FVector resultLocation;
	bool isHited;
#pragma endregion
};
