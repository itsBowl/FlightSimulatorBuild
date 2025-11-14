// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlightModelComponent.generated.h"


USTRUCT(BlueprintType)
struct FSurfaceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugging",
		meta = (ToolTip = "sets a fixed addend to the airspeed over the aerofoil"))
	float airspeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
	meta = (ToolTip = "Wing Area in m ^ 2"))
	float area = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Mean Aerodynamic chord, can be calculated or tuned"))
	float meanChord = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "location of the lifting surface relative to the CoM of the aircraft"))
	FVector location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Lift coefficient at zero angle of attack"))
	float Cl0 = .0f; //Cl0 = lift coefficent at zero angle of attack (aoa)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Lift coefficient per radian of aoa"))
	float ClAlpha = 1.f; //ClAlpha = lift coefficient per radian of aoa

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "Parasitic drag coefficient (drag produced by the body itself)"))
	float Cd0 = .02f; //parasitic drag coefficient

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "how the aspect of the craft effects drag"))
	float dragFactor = .045f; //factor for how aspect effects drag

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aerodynamics",
		meta = (ToolTip = "how effective is the control per radian aoa"))
	float controlFactor = .5f; //control effecitvness per rad aoa


};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSIMULATORBUILD_API UFlightModelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFlightModelComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight")
	UPrimitiveComponent* physicsBody = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	FSurfaceData wing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float tMax = 200000.f; // newtons

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float mass = 3000.f; // kilograms

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float airDensity = 1.225f; //kg/m^3 ASL

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	float throttle = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	float elevator = 0.f; //pitch (rads)

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	float aileron = 0.f; //roll (rads)

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	float rudder = 0.f; //yaw (rads)

	UPROPERTY(BlueprintReadWrite)
	AActor* localCamera = nullptr;


	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector getLocalVelocity() const;
	float getAirspeed() const;
	float getAirDensity() const;
	float computeAoA(const FVector&) const;
	float calculateAoA() const;
	void applyAero(const FVector&, float);
	void applyThrust(const FVector&, float);
	bool isFinite(FVector& vec)
	{
		return FMath::IsFinite(vec.X) && FMath::IsFinite(vec.Y) && FMath::IsFinite(vec.Z);
	}

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
