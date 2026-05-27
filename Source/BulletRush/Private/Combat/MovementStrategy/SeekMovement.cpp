#include "Combat/MovementStrategy/SeekMovement.h"
#include "GameFramework/Actor.h"

FVector USeekMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
    if (!Enemy) return FVector::ZeroVector;
    FVector Current = Enemy->GetActorLocation();
    FVector Dir = (TargetLocation - Current);
    float Dist = Dir.Size();
    if (Dist <= KINDA_SMALL_NUMBER) return Current;
    Dir.Normalize();
    return Current + Dir * Speed * DeltaTime;
}