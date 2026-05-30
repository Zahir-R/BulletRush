#pragma once

#include "CoreMinimal.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "HealthBonus.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UHealthBonus : public UPlayerStatsDecorator
{
	GENERATED_BODY()

protected:
	float ExtraHealth = 50.0f;
public:
	void SetExtraHealth(float InExtraHealth) { ExtraHealth = InExtraHealth; }
	virtual float GetMaxHealthBonus() const override
	{
		return InnerStats->GetMaxHealthBonus() + ExtraHealth;
	}
	virtual float GetHealthRestore() const override
	{
		return ExtraHealth;
	}
};
