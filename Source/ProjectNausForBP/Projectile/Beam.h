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

	void SetBeamProperty(ASpaceObject* launchActor, FVector targetedLocation, bool isWeapon, float setedDamage, float aliveTime = 1.0f);
#pragma region Beam Property
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Beam Data")
		UParticleSystemComponent* beamParticle;
#pragma endregion
};
