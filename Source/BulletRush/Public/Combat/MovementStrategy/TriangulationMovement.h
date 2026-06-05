#pragma once

#include "CoreMinimal.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "TriangulationMovement.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UTriangulationMovement : public UMovementStrat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TriangleSize = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StopDistance = 50.f;

	virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
	virtual void Reset() override;

private:
	TArray<FVector> Vertices;
	int32 VertexIndex = 0;
	bool bInitialized = false;
};
