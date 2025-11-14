// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "2Vector.h"
#include "AeroSurfaceConfig.h"


#include "AerodynamicSurface.generated.h"


USTRUCT(BlueprintType)
struct FAerodynamicSection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Spanwise offset from CoM of aircraft"))
	FVector position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Area of aerodynamic section"))
	float area = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Mean aerodynamic chord of aerodynamic section"))
	float chord = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Aerodynamic lift curve"))
	UCurveFloat* liftCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Aerodynamic drag curve"))
	UCurveFloat* dragCurve = nullptr;
};

UENUM(BlueprintType)
enum class EControlInputType : uint8
{
	Pitch UMETA(DisplayName = "Pitch"),
	Yaw UMETA(DisplayName = "Yaw"),
	Roll UMETA(DisplayName = "Roll"),
	Flaps UMETA(DisplayName = "Flaps")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSIMULATORBUILD_API UAerodynamicSurface : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAerodynamicSurface();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	FAeroSurfaceConfig config;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface")
	bool isControlSurface = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface", meta = (EditCondition = "isControlSurface", EditConditionHides))
	EControlInputType inputType = EControlInputType::Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamic Surface", meta = (EditCondition = "bIsControlSurface", EditConditionHides))
	float inputMultiplier = 1.0f;

#if WITH_EDITOR
	UStaticMeshComponent* surfaceMesh;
	UStaticMeshComponent* flapMesh;
	USceneComponent* flapHinge;
	UArrowComponent* upArrow;
	UMaterialInstanceDynamic* surfaceDynMat;
	UMaterialInstanceDynamic* flapDynMat;
#endif

	bool enableDebug = true;
	
	float flapAngle = 0.f;

	

	void setFlapAngle(float);
	FAeroVector calculateForces(const FVector&, float, const FVector&);

	FVector calculateCoefficients(float, float, float, float, float);
	FVector calculateCoefficientsAtLowAoA(float, float, float);
	FVector calculateCoefficientsAtStall(float, float, float, float, float);

	float torqueCoefficientProportion(float);
	float frictionAt90Degrees(float);
	float flapEffectivenessCorrection(float);
	float liftCoefficientMaxFraction(float);
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
