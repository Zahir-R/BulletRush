#pragma once
#include "CoreMinimal.h"

// Movement strategy interface for enemies
struct IMovementStrategy
{
    virtual ~IMovementStrategy() {}
    virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) = 0;
};

// Seek movement (linear)
struct FSeekMovement : public IMovementStrategy
{
    float Speed;
    FSeekMovement(float InSpeed = 300.f) : Speed(InSpeed) {}
    virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
};

// Sinusoidal seek movement
struct FSinusoidalSeekMovement : public IMovementStrategy
{
    float Amplitude;
    float Frequency;
    float StopDistance;
    float AccumulatedTime;
    FSinusoidalSeekMovement(float InAmplitude = 200.f, float InFrequency = 2.f, float InStopDistance = 500.f)
        : Amplitude(InAmplitude), Frequency(InFrequency), StopDistance(InStopDistance), AccumulatedTime(0.f) {}
    virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
};

// Static movement
struct FStaticMovement : public IMovementStrategy
{
    virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
};
