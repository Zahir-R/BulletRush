#include "Combat/MovementStrategy.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

FVector FSeekMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
    if (!Enemy) return FVector::ZeroVector;
    FVector Current = Enemy->GetActorLocation();
    FVector Dir = (TargetLocation - Current);
    float Dist = Dir.Size();
    if (Dist <= KINDA_SMALL_NUMBER) return Current;
    Dir.Normalize();
    FVector NewPos = Current + Dir * Speed * DeltaTime;
    return NewPos;
}

FVector FSinusoidalSeekMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
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
    // compute perpendicular vector (any)
    FVector Up = FVector::UpVector;
    FVector Perp = FVector::CrossProduct(ToTarget, Up);
    if (Perp.IsNearlyZero())
    {
        Perp = FVector::CrossProduct(ToTarget, FVector::RightVector);
    }
    Perp.Normalize();
    AccumulatedTime += DeltaTime;
    float Lateral = FMath::Sin(AccumulatedTime * Frequency * 2.f * PI) * Amplitude;
    FVector MoveDir = ToTarget + Perp * (Lateral / FMath::Max(1.f, Dist));
    MoveDir.Normalize();
    float BaseSpeed = 400.f; // default base speed
    FVector NewPos = Current + MoveDir * BaseSpeed * DeltaTime;
    return NewPos;
}

FVector FStaticMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
    if (!Enemy) return FVector::ZeroVector;
    return Enemy->GetActorLocation();
}
