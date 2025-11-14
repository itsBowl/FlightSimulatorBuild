// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftPhysicsComponent.h"

// Sets default values for this component's properties
UAircraftPhysicsComponent::UAircraftPhysicsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAircraftPhysicsComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->GetComponents<UAerodynamicSurface>(aeroSurfaces);
	physicsComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

	for (auto s : aeroSurfaces)
	{
		if (!s) return;
		s->enableDebug = enableDebug;
	}

	// ...
	
}


// Called every frame
void UAircraftPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!physicsComponent) return;

	const FVector linearVelocity = physicsComponent->GetPhysicsLinearVelocity();
	const FVector angularVelocity = physicsComponent->GetPhysicsAngularVelocityInRadians();
	const FVector CoM = physicsComponent->GetCenterOfMass();

	FAeroVector forceTorqueFrame = calculateAerodynamicForces(linearVelocity, angularVelocity, FVector::ZeroVector, airDensity, CoM);

	forceTorque = forceTorqueFrame;

	physicsComponent->AddForce(forceTorque.p / 100.f);
	physicsComponent->AddTorqueInRadians(forceTorque.q / 100.f);

	physicsComponent->AddForce(physicsComponent->GetForwardVector() * thrust * thrustPercent);

	// ...
}

FAeroVector UAircraftPhysicsComponent::calculateAerodynamicForces(const FVector& velocity, const FVector& angular, const FVector& wind, float density, const FVector& com)
{
	FAeroVector forces;
	for (auto s : aeroSurfaces)
	{
		FVector pos = s->GetComponentLocation() - com;
		FVector vel = -velocity + wind - FVector::CrossProduct(angular, pos);
		forces += s->calculateForces(vel, density, pos);
	}
	return forces;
}

void UAircraftPhysicsComponent::setThrustPercent(float p)
{
	thrustPercent = p;
}

