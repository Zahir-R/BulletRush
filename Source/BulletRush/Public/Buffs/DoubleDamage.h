#pragma once

#include "CoreMinimal.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "DoubleDamage.generated.h"

UCLASS()
class BULLETRUSH_API UDoubleDamage : public UPlayerStatsDecorator
{
	GENERATED_BODY()

public:
	virtual float GetDamageMultiplier() const override
	{
		return InnerStats ? InnerStats->GetDamageMultiplier() * 2.0f : 2.0f;
	}
};
