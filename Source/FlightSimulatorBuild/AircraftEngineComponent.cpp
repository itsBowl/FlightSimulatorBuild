// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftEngineComponent.h"



// Sets default values for this component's properties
UAircraftEngineComponent::UAircraftEngineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
#if WITH_EDITOR
	upArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	arrowHinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	if (upArrow)
	{
		upArrow->ArrowColor = FColor::Cyan;
		upArrow->ArrowSize = 1.f;
		upArrow->bHiddenInGame = false;
		upArrow->SetRelativeScale3D(FVector(1.f));
		FVector fwd = GetComponentTransform().GetRotation().GetForwardVector();
		upArrow->SetRelativeRotation(fwd.ToOrientationRotator());
	}
#endif
	// ...
}

void UAircraftEngineComponent::setThrustDirection(FVector dir) { direction = dir; }

void UAircraftEngineComponent::setThrustVector(float angle, EControlInputType type)
{
	angle = angle * 1.f;
	if (vectored)
	{
		FQuat rot;
		switch (type)
		{
		case (EControlInputType::Pitch):
			rot = FVector(0, angle, 0).ToOrientationQuat();
			this->SetRelativeRotation(rot);
			break;
		case (EControlInputType::Yaw):
			rot = FVector(0, 0, angle).ToOrientationQuat();
			this->SetRelativeRotation(rot);
			break;
		case (EControlInputType::Roll):
			rot = FVector(angle, 0, 0).ToOrientationQuat();
			this->SetRelativeRotation(rot);
			break;
		case (EControlInputType::Flaps):
			break;
		}
#if WITH_EDITOR
		upArrow->SetRelativeRotation(rot);
#endif	

		UE_LOG(LogTemp, Display, TEXT("%s"), *this->GetRelativeRotation().ToString());
	}
}

void UAircraftEngineComponent::setThrustVector(FVector a)
{
	//a = FMath::RadiansToDegrees(a);
	if (vectored)
	{
		FRotator rot = FRotator(FMath::RadiansToDegrees(-a.X), FMath::RadiansToDegrees(-a.Y), FMath::RadiansToDegrees(-a.Z));
		this->SetRelativeRotation(rot);
		UE_LOG(LogTemp, Display, TEXT("%s, %s"), *this->GetRelativeRotation().ToString(), *a.ToString());
	}
	
}

void UAircraftEngineComponent::setThrottle(float t) { throttle = t; }

FVector UAircraftEngineComponent::calculateForces()
{

	//UE_LOG(LogTemp, Display, TEXT("Vals: %f, %f, %s, %s"), throttle, thrust, *direction.ToString(), *GetComponentTransform().GetRotation().GetForwardVector().ToString())
	return throttle * thrust * direction;
}

float UAircraftEngineComponent::calculateThrust()
{
	return throttle * thrust;
}


// Called when the game starts
void UAircraftEngineComponent::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	
#endif

	// ...
	
}

void UAircraftEngineComponent::OnRegister()
{
	Super::OnRegister();
#if WITH_EDITOR
	arrowHinge->SetupAttachment(this);
	upArrow->SetupAttachment(arrowHinge);
#endif
}


// Called every frame
void UAircraftEngineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

