#pragma once
#include "CoreMinimal.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "PlayerSlowDecorator.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UPlayerSlowDecorator : public UPlayerStatsDecorator
{
	GENERATED_BODY()
public:
	virtual float GetSpeedMultiplier() const override { return InnerStats ? InnerStats->GetSpeedMultiplier() * 0.7f : 0.7f; }
	virtual float GetProjectileSpeedMultiplier() const override { return InnerStats ? InnerStats->GetProjectileSpeedMultiplier() * 0.7f : 0.7f; }
};
