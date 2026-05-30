#pragma once
#include "CoreMinimal.h"
#include "PlayerStatsDecorator.h"
#include "MassSlowDecorator.generated.h"

UCLASS(Blueprintable)
class UMassSlowDecorator : public UPlayerStatsDecorator
{
    GENERATED_BODY()
public:
    virtual float GetSpeedMultiplier() const override { return InnerStats ? InnerStats->GetSpeedMultiplier() * 0.5f : 0.5f; }
    virtual float GetProjectileSpeedMultiplier() const override { return InnerStats ? InnerStats->GetProjectileSpeedMultiplier() * 0.5f : 0.5f; }
};
