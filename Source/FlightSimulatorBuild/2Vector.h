#pragma once

#include "CoreMinimal.h"
#include "2Vector.generated.h"

USTRUCT(BlueprintType)
struct FAeroVector
{
	GENERATED_BODY()

public:
	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	FVector p = FVector::ZeroVector;

	UPROPERTY(EDITAnywhere, BlueprintReadWrite, Category = "Aerodynamics")
	FVector q = FVector::ZeroVector;

	FAeroVector() {}

	FAeroVector(const FVector& a, const FVector& b)
		: p(a), q(b)
	{
	}

	FORCEINLINE FAeroVector operator+(const FAeroVector& other) const
	{
		return FAeroVector(p + other.p, q + other.q);
	}

	friend FAeroVector operator*(float s, const FAeroVector& a)
	{
		return FAeroVector(s * a.p, s * a.q);
	}

	friend FAeroVector operator*(const FAeroVector& a, float s)
	{
		return s * a;
	}

	FORCEINLINE FAeroVector& operator+=(const FAeroVector& other)
	{
		p += other.p;
		q += other.q;
		return *this;
	}
};


UENUM(BlueprintType)
enum class EControlInputType : uint8
{
	Pitch UMETA(DisplayName = "Pitch"),
	Yaw UMETA(DisplayName = "Yaw"),
	Roll UMETA(DisplayName = "Roll"),
	Flaps UMETA(DisplayName = "Flaps")
};