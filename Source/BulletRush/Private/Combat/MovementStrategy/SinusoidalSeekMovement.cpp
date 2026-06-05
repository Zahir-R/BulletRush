#include "Combat/MovementStrategy/SinusoidalSeekMovement.h"
#include "GameFramework/Actor.h"

FVector USinusoidalSeekMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
    if (!Enemy) return FVector::ZeroVector;
    FVector Current = Enemy->GetActorLocation();
    FVector ToTarget = TargetLocation - Current;
    float Dist = ToTarget.Size();
    if (Dist <= StopDistance)
    {
        return Current;
    }
    ToTarget.Normalize();

    FVector Up = FVector::UpVector;
    FVector Perp = FVector::CrossProduct(ToTarget, Up);
    if (Perp.IsNearlyZero())
    {
        Perp = FVector::CrossProduct(ToTarget, FVector::RightVector);
    }
    Perp.Normalize();

    AccumulatedTime += DeltaTime;
    float Lateral = FMath::Sin(AccumulatedTime * Frequency * 2.f * PI + InitialPhase) * Amplitude;
    FVector MoveDir = ToTarget + Perp * (Lateral / FMath::Max(1.f, Dist));
    MoveDir.Normalize();

    float BaseSpeed = 400.f;
    return Current + MoveDir * BaseSpeed * DeltaTime;
}

void USinusoidalSeekMovement::Reset()
{
	Super::Reset();
	AccumulatedTime = 0.f;
}