#include "Combat/MovementStrategy/StaticMovement.h"
#include "GameFramework/Actor.h"

FVector UStaticMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
    if (!Enemy) return FVector::ZeroVector;
    return Enemy->GetActorLocation();
}

void UStaticMovement::Reset()
{
	Super::Reset();
}