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

	//Spring arm and camera
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArm->SetupAttachment(aircraftMesh);
	springArm->TargetArmLength = 600.f;
	springArm->bEnableCameraLag = true;
	springArm->CameraLagSpeed = 5.f;
	springArm->bDoCollisionTest = false;

	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	followCamera->SetupAttachment(springArm, USpringArmComponent::SocketName);

}

// Called when the game starts or when spawned
void AAircraftPawn::BeginPlay()
{
	Super::BeginPlay();

	this->GetComponents<UAerodynamicSurface>(surfaces);
	
}

// Called every frame
void AAircraftPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	updateControlSurfaces();

	if (aircraftPhysics)
	{
		aircraftPhysics->setThrustPercent(throttleInput);
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

