#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerStatsInterface.h"
#include "PlayerStatsBase.generated.h"

UCLASS(Blueprintable, BlueprintType)
class BULLETRUSH_API UPlayerStatsBase : public UObject, public IPlayerStatsInterface
{
	GENERATED_BODY()

public:
	virtual float GetDamageMultiplier() const { return 1.0f; }
	virtual float GetSpeedMultiplier() const { return 1.0f; }
	virtual float GetMaxHealthBonus() const { return 0.0f; }
	virtual float GetHealthRestore() const { return 0.0f; }	
	virtual void ChangeMesh() const { return;  }
};
