#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerStatsInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerStatsInterface : public UInterface
{
	GENERATED_BODY()
};

class BULLETRUSH_API IPlayerStatsInterface
{
	GENERATED_BODY()

public:
	virtual float GetDamageMultiplier() const = 0;
	virtual float GetSpeedMultiplier() const = 0;
	virtual float GetMaxHealthBonus() const = 0;
	virtual float GetHealthRestore() const = 0;
};
