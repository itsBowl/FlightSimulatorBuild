// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"


// Sets default values
AAircraftPawn::AAircraftPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//root mesh with physics sim
	aircraftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AircraftMesh"));
	SetRootComponent(aircraftMesh);
	aircraftMesh->SetSimulatePhysics(true);
	aircraftMesh->SetEnableGravity(true);
	aircraftMesh->SetLinearDamping(0.f);
	aircraftMesh->SetAngularDamping(0.f);

	//phsyics component time
	aircraftPhysics = CreateDefaultSubobject<UAircraftPhysicsComponent>(TEXT("AircraftPhysicsComponent"));
	if (!aircraftPhysics)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create physics context"));
	}

	//Spring arm and camera
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(aircraftMesh);
	springArm->TargetArmLength = 600.f;
	springArm->bEnableCameraLag = false;
	springArm->CameraLagSpeed = 5.f;
	springArm->bDoCollisionTest = false;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	followCamera->SetupAttachment(springArm, USpringArmComponent::SocketName);

#if WITH_EDITOR
	
#endif

}

// Called when the game starts or when spawned
void AAircraftPawn::BeginPlay()
{
	Super::BeginPlay();

	this->GetComponents<UAerodynamicSurface>(surfaces);
	this->GetComponents<UAircraftEngineComponent>(engines);
	if (!aircraftPhysics)
	{
		auto comp = GetComponentByClass(UAircraftPhysicsComponent::StaticClass());

		aircraftPhysics = Cast<UAircraftPhysicsComponent>(comp);
		UE_LOG(LogTemp, Warning, TEXT("Attempted to capture physics component"));
		if (aircraftPhysics)
			UE_LOG(LogTemp, Warning, TEXT("Captured physics"));
		if (!aircraftPhysics)
			UE_LOG(LogTemp, Warning, TEXT("Failed to capture physics"));

#if WITH_EDITOR

#endif
		
	}
	
}

// Called every frame
void AAircraftPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	updateControlSurfaces();
	updateEngineVectors();

	if (aircraftPhysics)
	{
		aircraftPhysics->setThrustPercent(throttleInput);
	}
	else if (!aircraftPhysics)
	{
		UE_LOG(LogTemp, Error, TEXT("NO PHYSICS"));
	}

}

// Called to bind functionality to input
void AAircraftPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAircraftPawn::updateControlSurfaces()
{
	if (!aircraftPhysics) return;

	for (auto s : surfaces)
	{
			if (!s || !s->isControlSurface)
				continue;
			switch (s->inputType)
			{
			case (EControlInputType::Pitch):
				s->setFlapAngle(pitchInput * s->inputMultiplier);
				break;
			case (EControlInputType::Yaw):
				s->setFlapAngle(yawInput * s->inputMultiplier);
				break;
			case (EControlInputType::Roll):
				s->setFlapAngle(rollInput * s->inputMultiplier);
				break;
			case (EControlInputType::Flaps):
				s->setFlapAngle(flapInput * s->inputMultiplier);
				break;

			}
	}
}

void AAircraftPawn::updateEngineVectors()
{
	for (auto e : engines)
	{
		e->setThrustVector(FVector(pitchInput, yawInput, rollInput));
	}
}

