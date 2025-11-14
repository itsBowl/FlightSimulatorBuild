// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AeroSurfaceConfig.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FAeroSurfaceConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float liftSlope = 6.28f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float skinFriction = .02;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float zeroLiftAoA = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float stallAngle = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float chord = 1.;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float flapFraction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float span = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	bool autoAspectRatio = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	float aspectRatio = 2.f;

	void validate()
	{
		flapFraction = FMath::Clamp(flapFraction, 0.f, .4f);
		stallAngle = FMath::Max(stallAngle, 0.f);
		chord = FMath::Max(chord, 1e-3f);

		if (autoAspectRatio) aspectRatio = span / chord;
	}

};
