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
	GetOwner()->GetComponents<UAircraftEngineComponent>(engines);
	physicsComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	int count = 0;

	for (auto s : aeroSurfaces)
	{
		if (!s)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get surface at index %d"), count);
			return;
		}
		s->enableDebug = enableDebug;
		UE_LOG(LogTemp, Display, TEXT("Captured control surface with name %s"), *s->GetName())
		count++;
	}

	count = 0;
	for (auto e : engines)
	{
		if (!e)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get engine at index %d"), count);
			return;
		}
		UE_LOG(LogTemp, Display, TEXT("Captured engine with name %s"), *e->GetName());
		//defaultThrustDirection(e, );
		count++;
	}

	// ...
	
}

void UAircraftPhysicsComponent::defaultThrustDirection(UAircraftEngineComponent* e, FVector d)
{
	e->setThrustDirection(d);
}


// Called every frame
void UAircraftPhysicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!physicsComponent) return;

	const FVector linearVelocity = physicsComponent->GetPhysicsLinearVelocity();
	const FVector simVel = FVector(simulatedVelocity, 0.f, 0.f);
	const FVector angularVelocity = physicsComponent->GetPhysicsAngularVelocityInRadians();
	const FVector CoM = physicsComponent->GetCenterOfMass();
	FBodyInstance* body = physicsComponent->GetBodyInstance();
	FVector CoG = body->GetCOMPosition();
	const FVector wind = FVector(0.f, 0.f, 0.f);
	FAeroVector forceTorqueFrame = calculateAerodynamicForces(linearVelocity, angularVelocity, wind, airDensity, CoM);
	FAeroVector engineForce = calculateEngineForces(CoM);

	forceTorque = forceTorqueFrame;

	physicsComponent->AddForce(forceTorque.p / 100.f);
	
	physicsComponent->AddTorqueInRadians(forceTorque.q / 100.f);
	//very temp fix needs to be completely redone at some point
	UAircraftEngineComponent* thisEngine = engines[0];
	
	
	
	//physicsComponent->AddForceAtLocation(engineForce.p, engineForce.q + physicsComponent->GetCenterOfMass());
	//physicsComponent->AddForceAtLocation(thisEngine->thrust * thisEngine->getForward() * thisEngine->throttle, GetOwner()->GetTransform().InverseTransformPositionNoScale(engineForce.q));
	
	//we can modify the add force location to be offset from CoM by swapping it out at some point in development
	//for now I want a thing what flies, so we're assuming every craft is a pusher from CoM directly

	physicsComponent->AddForceAtLocationLocal(engineForce.p, engineForce.q);

	//physicsComponent->AddForceAtLocation(engineForce.p, engineForce.q);

	
	UE_LOG(LogTemp, Display, TEXT("thrust: %s pos: %s thrust pos: %s"), *engineForce.p.ToString(), *CoM.ToString(), *(engineForce.q).ToString());

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

FAeroVector UAircraftPhysicsComponent::calculateEngineForces(const FVector& com)
{
	FAeroVector forces;
	for (auto e : engines)
	{
		forces.p += e->direction * e->calculateThrust();
		forces.q += e->GetRelativeLocation();;
		
	}

	return forces;
}

void UAircraftPhysicsComponent::setThrustPercent(float p)
{
	thrustPercent = p;
	for (auto e : engines)
	{
		e->setThrottle(p);
	}
}

