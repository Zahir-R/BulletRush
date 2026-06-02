#pragma once

#include "CoreMinimal.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "StaticMovement.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UStaticMovement : public UMovementStrat
{
	GENERATED_BODY()

public:
	virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
	virtual void Reset() override;
};