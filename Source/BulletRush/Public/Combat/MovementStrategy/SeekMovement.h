#pragma once

#include "CoreMinimal.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "SeekMovement.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API USeekMovement : public UMovementStrat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 300.f;

	virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
};