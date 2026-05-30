#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerStatsInterface.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UPlayerStatsInterface : public UInterface
{
    GENERATED_BODY()
};

class IPlayerStatsInterface
{
    GENERATED_BODY()
public:
    virtual float GetDamageMultiplier() const { return 1.0f; }
    virtual float GetSpeedMultiplier() const { return 1.0f; }
    virtual float GetMaxHealthBonus() const { return 0.f; }
    virtual float GetHealthRestore() const { return 0.f; }
    virtual float GetProjectileSpeedMultiplier() const { return 1.0f; }
};
