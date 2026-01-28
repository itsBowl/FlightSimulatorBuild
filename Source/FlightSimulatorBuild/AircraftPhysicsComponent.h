// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "AerodynamicSurface.h"
#include "AircraftEngineComponent.h"
#include "2Vector.h"
#include "AircraftPhysicsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSIMULATORBUILD_API UAircraftPhysicsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAircraftPhysicsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft Physics")
	float thrust = 300000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft Physics")
	TArray<UAerodynamicSurface*> aeroSurfaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft Physics")
	TArray<UAircraftEngineComponent*> engines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft Physics")
	float airDensity = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft Physics")
	float predictionTimestepFraction = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aircraft Physics")
	bool enableDebug = true;

	float simulatedVelocity = 0.f;

	void setThrustPercent(float);
	void defaultThrustDirection(UAircraftEngineComponent*, FVector);
	

	float thrustPercent = 0.f;
	FAeroVector forceTorque;

	UPrimitiveComponent* physicsComponent = nullptr;
	UPrimitiveComponent* root = nullptr;

	FAeroVector calculateAerodynamicForces(const FVector&, const FVector&, const FVector&, float, const FVector&);
	FAeroVector calculateEngineForces(const FVector&);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
