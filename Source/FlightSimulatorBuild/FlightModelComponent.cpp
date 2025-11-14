// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightModelComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFlightModelComponent::UFlightModelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

#define DBGMSG(x, color, ...) if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, color, FString::Printf(TEXT(x), __VA_ARGS__)); }


// Called when the game starts
void UFlightModelComponent::BeginPlay()
{
	Super::BeginPlay();
	auto owner = GetOwner();
	UE_LOG(LogTemp, Display, TEXT("Owner name: %s"), *owner->GetName());
	UE_LOG(LogTemp, Display, TEXT("Root name: %s"), *owner->GetRootComponent()->GetName());
	if (!physicsBody)
	{
		physicsBody = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Found physics body"));
	}

	

	if (physicsBody)
	{
		UE_LOG(LogTemp, Display, TEXT("Physics body name: %s"), *physicsBody->GetName());
		physicsBody->SetMassOverrideInKg(NAME_None, mass);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get a physics body"));
	}
	
	
}


// Called every frame
void UFlightModelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!physicsBody) return;

	

	FVector body = getLocalVelocity();
	applyAero(body, DeltaTime);
	applyThrust(body, DeltaTime);
	
}

FVector UFlightModelComponent::getLocalVelocity() const
{
	if (!physicsBody) return FVector::ZeroVector;

	FVector world = physicsBody->GetComponentVelocity() * 0.01f; //convert to m/s
	FTransform transform = physicsBody->GetComponentTransform();
	FVector vel = transform.InverseTransformVectorNoScale(world);
	//UE_LOG(LogTemp, Display, TEXT("Velocity: %s"), *(vel.ToString()));
	return vel;
}

float UFlightModelComponent::getAirspeed() const
{
	return getLocalVelocity().Size();
}

float UFlightModelComponent::getAirDensity() const
{
	/*
	// p = pb * exp(-g0 * M0 (H - Hb) / R* * Tmb) 
	// or (more accurate)
	// p = pb * (Tmb/Tmb + lmb * (H - Hb)) ^ (g0 * m0 / R* * Lmb)
	// p = rho = pressure
	// pb = reference pressure (1.225 kg/m^3 ASL)
	// Tmb = reference temperature (
	// H = altitude
	// Hb = altitude of reference level pb (0m)
	// M0 = mean molecular mass of air ASL (28.9644kg/kmol)
	// R* = universal gas constant (8314.32 [8.31432x10^3] Nm/kmolK)
	// g0 = 9.80665 (gravitational accelleration
	//
	
	big table of data for the complex version
	Subscript b	|Geopotential	|Static pressure	|Standard temperature	|Temperature Gradient	|Exponent
				|(km)			|(Pa)				|(K)					|(K/km)					|g0 M / R L
	===========================================================================================================
	0			|0				|101 325			|288.15					|-6.5					|-5.25588
	1			|11				|22 632.1			|216.65					|0						|N/A
	2			|20				|5 474.89			|216.65					|1						|34.1626
	3			|32				|868.019			|228.65					|2.8					|12.2009
	4			|47				|110.9063			|270.65					|0						|N/A
	5			|51				|66.9389			|270.65					|-2.8					|-12.2009
	6			|71				|3.95642			|214.65					|-2						|-17.0813
	*/

	//simple
	float altitude = physicsBody->GetComponentLocation().Z * 0.01f; //convert UE units (cm) to sci (m)
	float rho = 1.225f;
	float num = -9.80665 * (28.9644 * altitude);
	float denom = 8314.32 * 288.15;
	float frac = num / denom;
	float exp = FMath::Exp(frac);
	return rho * exp;
}

float UFlightModelComponent::computeAoA(const FVector& body) const
{
	//assuimg +X forward, +Z up so make sure this is correct
	//this bit here is also wrong, since we're not accounting for the angle of the aerofoil wrt the velocity
	if (FMath::IsNearlyZero(body.X)) return 0.0f;
	return FMath::Atan2(body.Z, body.X); //rads

	

}

float UFlightModelComponent::calculateAoA() const
{
	/*
	aoa = angle between incoming airmass (aka, velocity vector of the plane) and the angle of the aerofoid chord
	for the sake of making this calculation easier to think about for now, we're going to make the assumtion
	that the aerofoil chord is the angle that the physics actor holds
	to calculate the angle between our velocity and that, we need to get both, then return
	*/
	FVector CoM = physicsBody->GetCenterOfMass();
	FVector velocity = physicsBody->GetComponentVelocity();
	FVector nose = physicsBody->GetForwardVector();
	//DBGMSG("Nose angle: %s", FColor::Red, *nose.ToString());
	DrawDebugLine(GetWorld(), CoM, CoM + (velocity), FColor::Yellow, false, 0.001f, 255, 4.f);
	DrawDebugLine(GetWorld(), CoM, CoM + (nose.GetSafeNormal() * 100), FColor::Cyan, false, 0.001f, 0, 4.f);
	auto num = velocity.Dot(nose);
	auto denom = velocity.Length() * nose.Length();
	float aoa = FMath::Acos(num / denom);
	//DBGMSG("AoA: %f", FColor::Red, aoa);
	return aoa;
}

void UFlightModelComponent::applyAero(const FVector& body, float dt)
{

	float rho = getAirDensity();
	float v = body.Size();
	v += wing.airspeed;
	//if (v < KINDA_SMALL_NUMBER) return;

	//dynamic pressure
	float q = 0.5f * rho * (v * v);

	float alpha = calculateAoA();

	float Cl = wing.Cl0 + (wing.ClAlpha * alpha * wing.controlFactor) /** elevator*/;
	//limit to simulate airfoil salling
	
	float k = 6.f;
	float clPreStall = FMath::Sqrt(1.0f + (k * alpha) * (k * alpha));
	//Cl = Cl / FMath::Sqrt(1.0f + (k * alpha) * (k * alpha));
	

	float Cd = wing.Cd0 + wing.dragFactor * Cl * Cl;

	float lift = q * wing.area * Cl;
	float drag = q * wing.area * Cd;

	FVector relativeWindNormal = -physicsBody->GetComponentVelocity().GetSafeNormal();

	
	FTransform transform = physicsBody->GetComponentTransform();
	FVector right = transform.GetUnitAxis(EAxis::Type(2));
	FVector up = transform.GetUnitAxis(EAxis::Type(0));
	FVector forward = transform.GetUnitAxis(EAxis::Type(1));
	FVector upBody = transform.InverseTransformVectorNoScale(up).GetSafeNormal();
	

	
	//FVector rightBodyWorld = transform.TransformVectorNoScale(rightBody);
	FVector rb = transform.InverseTransformVectorNoScale(right).GetSafeNormal();

	//lift vector is always wing relative up, because that's how the geometry works out
	//lift is generated by the wing geometry, not the direction of the wind
	FVector liftDir = up;//FVector::CrossProduct(right, relativeWindNormal);
	//liftDir = up;
	//liftDir = liftDir.GetSafeNormal();
	
	
	if (!isFinite(liftDir))
	{
		liftDir = FVector::UpVector; //fallback case
		DBGMSG("BADLIFT %i", FColor::Red, 1);
	}
	

	FVector liftBody = liftDir * lift;
	FVector dragBody = relativeWindNormal * drag;
	FVector CoM = physicsBody->GetCenterOfMass();

	FVector wingLocation = transform.TransformPosition(wing.location);
	physicsBody->AddForceAtLocation(liftBody, wingLocation);
	//DBGMSG("CoM: %s", FColor::Red, *CoM.ToString());
	

	//pitching moment
	float CmAlpha = -0.02f;
	float Cm = (CmAlpha * alpha) + (-0.01f) * elevator;
	float pitchMoment = q * wing.area * wing.meanChord * Cm;

	FVector torqueBody = FVector(0, pitchMoment, 0);
	FVector worldTorque = (torqueBody);

	//physicsBody->AddTorqueInRadians(worldTorque, NAME_None, true);
	

#if WITH_EDITOR

	//DrawDebugLine(GetWorld(), wingLocation, wingLocation + (liftDir * 100.f), FColor::Magenta, false, 0.001f, 0, 4.f);
	DrawDebugLine(GetWorld(), wingLocation, wingLocation + (liftBody), FColor::Green, false, 0.001f, 0, 4.f);
	DrawDebugLine(GetWorld(), wingLocation, wingLocation + (torqueBody.GetSafeNormal() * 100.f), FColor::Cyan, false, 0.001f, 0, 4.f);
	
	//UE_LOG(LogTemp, Display, TEXT("this: %s = alpha: %f, q: %f, Moment: %f"), *this->GetName(), alpha, q, pitchMoment);
	//DrawDebugPoint(GetWorld(), wingLocation, 4.f, FColor::Black, false, 0.001f, 255);
	
#endif
	//physicsBody->AddForce(FVector(0, 0, 10000000));
	
}

void UFlightModelComponent::applyThrust(const FVector& body, float dt)
{
	float forward = body.X;
	//very simple and doesn't really take into account aero but we fix this later
	float thrust = throttle * tMax * FMath::Clamp(1.0f - 0.001f * forward, 0.0f, 1.0f);
	FTransform transform = physicsBody->GetComponentTransform();
	FVector thrustWorld = transform.TransformVectorNoScale(FVector(thrust, 0, 0));
	FVector forcePoint = transform.TransformPosition(FVector::ZeroVector); //use position of engines here
	physicsBody->AddForceAtLocation(thrustWorld, forcePoint);

}

