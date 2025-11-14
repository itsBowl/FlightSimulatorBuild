// Fill out your copyright notice in the Description page of Project Settings.


#include "AerodynamicSurface.h"

// Sets default values for this component's properties
UAerodynamicSurface::UAerodynamicSurface()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

#if WITH_EDITOR
	surfaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SurfaceMesh"));
	flapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlapMesh"));
	upArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	flapHinge = CreateDefaultSubobject<USceneComponent>(TEXT("FLapHinge"));

	UStaticMesh* planeMesh = LoadObject<UStaticMesh>(nullptr, 
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	UMaterialInstance* visualMat = LoadObject<UMaterialInstance>(nullptr, 
		TEXT("/Game/FlightSimulation/Visualiser/M_SurfaceVisual_Inst.M_SurfaceVisual_Inst"));
	
	if (!planeMesh)
		UE_LOG(LogTemp, Error, TEXT("not found visualiser plane"));
	if (!visualMat)
		UE_LOG(LogTemp, Error, TEXT("not found visualiser material"));
	if (planeMesh)
		UE_LOG(LogTemp, Error, TEXT("found visualiser plane"));
	if (visualMat)
		UE_LOG(LogTemp, Error, TEXT("found visualiser material"));
		

	if (planeMesh)
	{
		if (surfaceMesh)
		{
			surfaceMesh->SetStaticMesh(planeMesh);
			surfaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			surfaceMesh->SetMobility(EComponentMobility::Movable);
			surfaceMesh->bHiddenInGame = false;

			if (visualMat)
			{
				surfaceDynMat = UMaterialInstanceDynamic::Create(visualMat, this);
				surfaceDynMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0, 0, .5f));
				surfaceMesh->SetMaterial(0, surfaceDynMat);
			}
		}

		if (flapMesh)
		{
			flapMesh->SetStaticMesh(planeMesh);
			flapMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			flapMesh->SetMobility(EComponentMobility::Movable);
			flapMesh->bHiddenInGame = false;

			if (visualMat)
			{
				flapDynMat = UMaterialInstanceDynamic::Create(visualMat, this);
				flapDynMat->SetVectorParameterValue(TEXT("Color"), FLinearColor::Yellow);
				flapMesh->SetMaterial(0, flapDynMat);
			}
		}

		if (upArrow)
		{
			upArrow->ArrowColor = FColor::Cyan;
			upArrow->ArrowSize = .5f;
			upArrow->bHiddenInGame = false;
			upArrow->SetRelativeScale3D(FVector(0.5f));
			upArrow->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		}
	}
#endif
}

void UAerodynamicSurface::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	if (!surfaceMesh || !flapMesh)
		return;
	surfaceMesh->SetupAttachment(this);
	flapHinge->SetupAttachment(this);
	flapMesh->SetupAttachment(flapHinge);
	upArrow->SetupAttachment(this);

	const float surfaceChord = config.chord * (1.0f - config.flapFraction);
	const float flapChord = config.chord * config.flapFraction;

	FVector parentWorldScale = GetComponentTransform().GetScale3D();
	const float gapFraction = .01f;
	const float gap = .5f * config.chord * gapFraction * 100.f;

	float surfaceHalfLength = .5f * surfaceChord / parentWorldScale.Y * 100.f;
	float flapHalfLength = .5f * flapChord / parentWorldScale.Y * 100.f;

	FVector surfaceDesiredScale(config.span / parentWorldScale.X, 
		surfaceChord / parentWorldScale.Y, 1.f / parentWorldScale.Z);

	surfaceMesh->SetRelativeScale3D(surfaceDesiredScale);
	surfaceMesh->SetRelativeLocation(FVector(0.f, -flapHalfLength - gap, 0.f));

	flapHinge->SetRelativeLocation(FVector(0.f, -flapHalfLength + surfaceHalfLength, 0.f));

	flapMesh->SetRelativeScale3D(FVector(config.span / parentWorldScale.X,
		flapChord / parentWorldScale.Y, 1.f / parentWorldScale.Z));
	flapMesh->SetRelativeLocation(FVector(0.f, flapHalfLength, 0.f));
#endif
}


// Called when the game starts
void UAerodynamicSurface::BeginPlay()
{
	Super::BeginPlay();

	if (surfaceMesh)
		surfaceMesh->SetVisibility(enableDebug);
	if (flapMesh)
		surfaceMesh->SetVisibility(enableDebug);
	if (upArrow)
		surfaceMesh->SetVisibility(isControlSurface && enableDebug);
}

void UAerodynamicSurface::setFlapAngle(float angle)
{
	float newAngle = FMath::Clamp(angle, -FMath::DegreesToRadians(50.f), FMath::DegreesToRadians(50.f));
	flapAngle = newAngle;
#if WITH_EDITOR
	if (enableDebug)
	{
		const FRotator flapRotation = FRotator(0.f, 0.f, FMath::RadiansToDegrees(flapAngle));
		UE_LOG(LogTemp, Display, TEXT("FlapAngle for %s is: %f. Requested angle: %f, SetAngle: %f"), 
			*this->GetName(), FMath::RadiansToDegrees(flapAngle), FMath::RadiansToDegrees(angle), FMath::RadiansToDegrees(newAngle));
		flapHinge->SetRelativeRotation(flapRotation);
	}
#endif	
}

FAeroVector UAerodynamicSurface::calculateForces(const FVector& airVelocity, float airDensity, const FVector& relativePosition)
{
	FAeroVector forceTorque;

	config.validate();

	const float correctedLiftSlope = config.liftSlope * config.aspectRatio /
		(config.aspectRatio + 2 * (config.aspectRatio + 4) / (config.aspectRatio + 2));

	const float theta = FMath::Acos(2 * config.flapFraction - 1);
	const float flapEffectiveness = 1 - (theta - FMath::Sin(theta)) / PI;
	const float deltaLift = correctedLiftSlope * flapEffectiveness * flapEffectivenessCorrection(flapAngle) * flapAngle;

	const float zeroLiftAoABase = FMath::DegreesToRadians(config.zeroLiftAoA);
	const float zeroLiftAoA = zeroLiftAoABase - deltaLift / correctedLiftSlope;

	const float stallAngleHighBase = FMath::DegreesToRadians(config.stallAngle);
	const float stallAngleLowBase = FMath::DegreesToRadians(-config.stallAngle);

	const float ClMaxHigh = correctedLiftSlope * (stallAngleHighBase - zeroLiftAoABase) + deltaLift * liftCoefficientMaxFraction(config.flapFraction);
	const float ClMaxLow = correctedLiftSlope * (stallAngleLowBase - zeroLiftAoABase) + deltaLift * liftCoefficientMaxFraction(config.flapFraction);

	const float stallHigh = zeroLiftAoA + ClMaxHigh / correctedLiftSlope;
	const float stallLow = zeroLiftAoA + ClMaxLow / correctedLiftSlope;

	FVector localAirVelocity = GetComponentTransform().InverseTransformVectorNoScale(airVelocity);
	localAirVelocity = FVector(0., localAirVelocity.Y, localAirVelocity.Z);

	const FVector dragDir = GetComponentTransform().TransformVectorNoScale(localAirVelocity.GetSafeNormal());
	
	const FVector liftDir = FVector::CrossProduct(dragDir, -GetForwardVector());

	const float area = config.chord * config.span;
	const float dynamicPressure = .5f * airDensity * localAirVelocity.SizeSquared();
	const float AoA = FMath::Atan2(localAirVelocity.Z, localAirVelocity.Y);

	const FVector coeffs = calculateCoefficients(AoA, correctedLiftSlope, zeroLiftAoA, stallHigh, stallLow);

	const FVector lift = liftDir * coeffs.X * dynamicPressure * area;
	const FVector drag = dragDir * coeffs.Y * dynamicPressure * area;
	const FVector torque = GetForwardVector() * coeffs.Z * dynamicPressure * area * config.chord;

	forceTorque.p += lift + drag;
	forceTorque.q += FVector::CrossProduct(relativePosition, forceTorque.p);
	forceTorque.q += torque;

	if (enableDebug)
	{
		const float visualScaleF = 0.00003f;
		const FVector worldCoM = GetComponentTransform().TransformPosition(FVector::ZeroVector);
		DrawDebugDirectionalArrow(GetWorld(), worldCoM, worldCoM + lift * visualScaleF, 20.f, FColor::Green, false, 0.f, 0, 2.f);
		DrawDebugDirectionalArrow(GetWorld(), worldCoM, worldCoM + drag * visualScaleF, 20.f, FColor::Red, false, 0.f, 0, 2.f);
		DrawDebugDirectionalArrow(GetWorld(), worldCoM, worldCoM + torque * visualScaleF, 20.f, FColor::Blue, false, 0.f, 0, 2.f);
	}

	return forceTorque;
}

FVector UAerodynamicSurface::calculateCoefficients(float aoa, float cls, float zeroLift, float stallHigh, float stallLow)
{
	FVector coeffs;
	const float paddingHigh = FMath::DegreesToRadians(FMath::Lerp(15.f, 5.f, (FMath::RadiansToDegrees(flapAngle + 50) / 100.f)));
	const float paddingLow = FMath::DegreesToRadians(FMath::Lerp(15.f, 5.f, (-FMath::RadiansToDegrees(flapAngle + 50) / 100.f)));
	const float paddedHigh = stallHigh + paddingHigh;
	const float paddedLow = stallLow + paddingLow;

	if (aoa < stallHigh && aoa > stallLow)
	{
		coeffs = calculateCoefficientsAtLowAoA(aoa, cls, zeroLift);
	}
	else
	{
		if (aoa > paddedHigh || aoa < paddedLow)
		{
			coeffs = calculateCoefficientsAtStall(aoa, cls, zeroLift, stallHigh, stallLow);
		}
		else
		{
			FVector coeffsLow, coeffsStall;
			float lerpT = 0;
			if (aoa > stallHigh)
			{
				coeffsLow = calculateCoefficientsAtLowAoA(stallHigh, cls, zeroLift);
				coeffsStall = calculateCoefficientsAtStall(paddedHigh, cls, zeroLift, stallHigh, stallLow);
				lerpT = (aoa - stallHigh) / (paddingHigh - stallHigh);
			}
			else
			{
				coeffsLow = calculateCoefficientsAtLowAoA(stallLow, cls, zeroLift);
				coeffsStall = calculateCoefficientsAtStall(paddedLow, cls, zeroLift, stallHigh, stallLow);
				lerpT = (aoa - stallLow) / (paddedLow - stallLow);
			}
			coeffs = FMath::Lerp(coeffsLow, coeffsStall, lerpT);
		}
	}

#if WITH_EDITOR
	if (enableDebug)
	{
		const bool isAtStall = !(aoa < stallHigh && aoa > stallLow);
		surfaceDynMat->SetVectorParameterValue(TEXT("Color"), isAtStall ? FLinearColor(1.f, 0, 0) : FLinearColor(0, 0, .5f));
		flapDynMat->SetVectorParameterValue(TEXT("Color"), isAtStall ? FLinearColor(1.f, 0, 0) : FLinearColor::Yellow);
	}
#endif
	return coeffs;
}

FVector UAerodynamicSurface::calculateCoefficientsAtLowAoA(float aoa, float cls, float zeroLift)
{
	const float lift = cls * (aoa - zeroLift);
	const float inducedAngle = lift / (PI * config.aspectRatio);
	const float effAngle = aoa - zeroLift - inducedAngle;

	const float tan = config.skinFriction * FMath::Cos(effAngle);
	const float norm = (lift + FMath::Sin(effAngle) * tan) / FMath::Cos(effAngle);
	const float drag = norm * FMath::Sin(effAngle) + tan * FMath::Cos(effAngle);
	const float torque = -norm * torqueCoefficientProportion(effAngle);

	return FVector(lift, drag, torque);
}

FVector UAerodynamicSurface::calculateCoefficientsAtStall(float aoa, float cls, float zeroLift, float stallH, float stallL)
{
	const float liftAoa = (aoa > stallH)
		? cls * (stallH - zeroLift)
		: cls * (stallL - zeroLift)
		;
	float inducedAngle = liftAoa / (PI * config.aspectRatio);
	float lerpT = 0;

	if (aoa > stallH)
		lerpT = (HALF_PI - FMath::Clamp(aoa, -HALF_PI, HALF_PI)) / (HALF_PI - stallH);
	else
		lerpT = (-HALF_PI - FMath::Clamp(aoa, -HALF_PI, HALF_PI)) / (-HALF_PI - stallL);

	inducedAngle = FMath::Lerp(0.f, inducedAngle, FMath::Clamp(lerpT, 0, 1));
	const float effAngle = aoa - zeroLift - inducedAngle;

	const float norm = frictionAt90Degrees(flapAngle) * FMath::Sin(effAngle) *
		(1.f / (.56f + .44f * FMath::Abs(FMath::Sin(effAngle))) -
			0.41f * (1.f - FMath::Exp(-17.f / config.aspectRatio)));
	const float tan = .5f * config.skinFriction * FMath::Cos(effAngle);

	const float lift = norm * FMath::Cos(effAngle) - tan * FMath::Sin(effAngle);
	const float drag = norm * FMath::Sin(effAngle) + tan * FMath::Cos(effAngle);
	const float torque = -norm * torqueCoefficientProportion(effAngle);
	return FVector(lift, drag, torque);
}

float UAerodynamicSurface::torqueCoefficientProportion(float f)
{
	return 0.25f - 0.175f * (1 - 2 * FMath::Abs(f) / PI);
}

float UAerodynamicSurface::frictionAt90Degrees(float f)
{
	return 1.98f - 0.0426f * f * f * 0.21f * f;
}

float UAerodynamicSurface::flapEffectivenessCorrection(float f)
{
	return FMath::Lerp(.8f, 0.4f, (FMath::RadiansToDegrees(FMath::Abs(f)) - 10) / 50);
}

float UAerodynamicSurface::liftCoefficientMaxFraction(float f)
{
	return FMath::Clamp(1 - .5f * (f - .1f) / .3f, 0.f, 1.f);
}


// Called every frame
void UAerodynamicSurface::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

