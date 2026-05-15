#pragma once

#include "CoreMinimal.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "SpeedBoost.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API USpeedBoost : public UPlayerStatsDecorator
{
	GENERATED_BODY()

protected:
	float Multiplier = 2.0f;
public:
	void SetMultiplier(float InMultiplier) { Multiplier = InMultiplier; }
	virtual float GetSpeedMultiplier() const override
	{
		return InnerStats->GetSpeedMultiplier() * Multiplier;
	}
};
