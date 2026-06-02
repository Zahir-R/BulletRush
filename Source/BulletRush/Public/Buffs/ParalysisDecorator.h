#pragma once
#include "CoreMinimal.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "ParalysisDecorator.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UParalysisDecorator : public UPlayerStatsDecorator
{
	GENERATED_BODY()
public:
	virtual float GetSpeedMultiplier() const override { return 0.f; }
	virtual float GetProjectileSpeedMultiplier() const override { return 0.f; }
};
