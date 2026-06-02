#include "Combat/MovementStrategy/AscendMovement.h"
#include "GameFramework/Actor.h"

FVector UAscendMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
	if (!Enemy) return FVector::ZeroVector;

	FVector Current = Enemy->GetActorLocation();

	if (!bInitialized)
	{
		StartZ = Current.Z;
		bInitialized = true;
	}

	float NewZ = Current.Z + Speed * DeltaTime;
	if (NewZ - StartZ >= MaxDelta)
	{
		NewZ = StartZ + MaxDelta;
		bCompleted = true;
		Current.Z = NewZ;
		return Current;
	}

	Current.Z = NewZ;
	return Current;
}

void UAscendMovement::Reset()
{
	Super::Reset();
	StartZ = 0.f;
	bInitialized = false;
}
