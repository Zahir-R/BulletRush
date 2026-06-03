#pragma once

#include "CoreMinimal.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "DescendMovement.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UDescendMovement : public UMovementStrat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxDelta = 300.f;

	virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) override;
	virtual void Reset() override;

private:
	float StartZ = 0.f;
	bool bInitialized = false;
};
