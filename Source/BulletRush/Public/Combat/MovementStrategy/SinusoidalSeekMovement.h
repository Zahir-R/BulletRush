#pragma once

#include "CoreMinimal.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "SinusoidalSeekMovement.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API USinusoidalSeekMovement : public UMovementStrat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Amplitude = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Frequency = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StopDistance = 500.f;

public:
	float InitialPhase = 0.f;

private:
	float AccumulatedTime = 0.f;

public:
	virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
};