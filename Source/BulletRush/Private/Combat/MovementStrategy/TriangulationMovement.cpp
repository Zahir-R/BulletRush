#include "Combat/MovementStrategy/TriangulationMovement.h"
#include "GameFramework/Actor.h"

FVector UTriangulationMovement::GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation)
{
	if (!Enemy) return FVector::ZeroVector;

	FVector Current = Enemy->GetActorLocation();

	if (!bInitialized)
	{
		Vertices.Empty();
		FVector Start = Current;
		Start.Z = 0.f;

		float S = TriangleSize;
		float H = S * 0.866f;
		Vertices.Add(Start + FVector(S, 0.f, 0.f));
		Vertices.Add(Start + FVector(-S * 0.5f, H, 0.f));
		Vertices.Add(Start + FVector(-S * 0.5f, -H, 0.f));

		VertexIndex = 0;
		bInitialized = true;
	}

	if (VertexIndex >= 3)
	{
		bCompleted = true;
		return Enemy->GetActorLocation();
	}

	FVector Target = Vertices[VertexIndex];
	Target.Z = Current.Z;

	FVector Dir = (Target - Current).GetSafeNormal();
	float Dist = FVector::Dist(Current, Target);

	if (Dist <= StopDistance)
	{
		VertexIndex++;
		return Target;
	}

	return Current + Dir * Speed * DeltaTime;
}

void UTriangulationMovement::Reset()
{
	Super::Reset();
	Vertices.Empty();
	VertexIndex = 0;
	bInitialized = false;
}
