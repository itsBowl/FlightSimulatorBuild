// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AircraftPhysicsComponent.h"
#include "AerodynamicSurface.h"

#include "AircraftPawn.generated.h"

UCLASS()
class FLIGHTSIMULATORBUILD_API AAircraftPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAircraftPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aircraft")
	UStaticMeshComponent* aircraftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aircraft")
	UAircraftPhysicsComponent* aircraftPhysics;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aircraft")
	class USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aircraft")
	class UCameraComponent* followCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aircraft Controls")
	float throttleInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aircraft Controls")
	float pitchInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aircraft Controls")
	float yawInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aircraft Controls")
	float rollInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aircraft Controls")
	float flapInput = 0.f;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	TArray<UAerodynamicSurface*> surfaces;

	void updateControlSurfaces();



};
