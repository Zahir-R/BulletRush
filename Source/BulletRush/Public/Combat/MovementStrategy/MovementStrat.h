#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MovementStrat.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class BULLETRUSH_API UMovementStrat : public UObject
{
	GENERATED_BODY()

public:
	virtual FVector GetNextPosition(AActor* Enemy, float DeltaTime, const FVector& TargetLocation) PURE_VIRTUAL(UMovementStrat::GetNextPosition, return FVector::ZeroVector;);
};