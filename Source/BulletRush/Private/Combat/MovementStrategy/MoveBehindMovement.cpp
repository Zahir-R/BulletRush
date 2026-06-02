#include "Combat/MovementStrategy/MoveBehindMovement.h"
#include "GameFramework/Actor.h"

FVector UMoveBehindMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
	if (!Enemy) return FVector::ZeroVector;

	FVector Current = Enemy->GetActorLocation();
	FVector Dir = (TargetLocation - Current);
	float Dist = Dir.Size();
	if (Dist <= StopDistance)
	{
		bCompleted = true;
		return Current;
	}
	Dir.Normalize();
	return Current + Dir * Speed * DeltaTime;
}
