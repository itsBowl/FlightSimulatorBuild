// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "2Vector.h"
#include "AircraftEngineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSIMULATORBUILD_API UAircraftEngineComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAircraftEngineComponent();

	UArrowComponent* upArrow;
	USceneComponent* arrowHinge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Properties")
	bool vectored = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Engine Properties", meta = (ToolTip = "this is converted into unreal units (I FUCKING HATE cN WITH A BURNING PASSION) internally"))
	float thrust = 30000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Properties")
	float throttle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Properties")
	FVector direction = FVector(0.f, 0.f, 1.f);

	void setThrustDirection(FVector);
	void setThrottle(float);
	FVector calculateForces();

	void setThrustVector(float, EControlInputType);
	void setThrustVector(FVector);

	inline FVector getForward() { return GetComponentTransform().GetRotation().GetForwardVector(); }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
