#pragma once

#include "CoreMinimal.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "MoveBehindMovement.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UMoveBehindMovement : public UMovementStrat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Distance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StopDistance = 50.f;

	virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
};
