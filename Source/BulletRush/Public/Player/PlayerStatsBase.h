#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerStatsInterface.h"
#include "PlayerStatsBase.generated.h"

UCLASS()
class BULLETRUSH_API UPlayerStatsBase : public UObject, public IPlayerStatsInterface
{
	GENERATED_BODY()

public:
	virtual float GetDamageMultiplier() const override { return 1.0f; }
	virtual float GetSpeedMultiplier() const override { return 1.0f; }
	virtual float GetMaxHealthBonus() const override { return 0.0f; }
	virtual float GetHealthRestore() const override { return 0.0f; }	
};
