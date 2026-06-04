#pragma once
#include "CoreMinimal.h"
#include "PlayerStatsDecorator.h"
#include "SlowDecorator.generated.h"

UCLASS(Blueprintable)
class USlowDecorator : public UPlayerStatsDecorator
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slow")
    float SlowFactor = 0.5f;

    virtual float GetSpeedMultiplier() const override { return InnerStats ? InnerStats->GetSpeedMultiplier() * SlowFactor : SlowFactor; }
    virtual float GetProjectileSpeedMultiplier() const override { return InnerStats ? InnerStats->GetProjectileSpeedMultiplier() * SlowFactor : SlowFactor; }
};
