#pragma once

#include "CoreMinimal.h"
#include "Player/PlayerStatsBase.h"
#include "PlayerStatsDecorator.generated.h"

UCLASS(Abstract)
class BULLETRUSH_API UPlayerStatsDecorator : public UPlayerStatsBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TScriptInterface<IPlayerStatsInterface> InnerStats;

public:
	void SetInner(TScriptInterface<IPlayerStatsInterface> InInner) { InnerStats = InInner; }

	TScriptInterface<IPlayerStatsInterface> GetInnerStats() const { return InnerStats; }

	virtual float GetDamageMultiplier() const override 
	{
		return InnerStats ? InnerStats->GetDamageMultiplier() : 1.0f;
	}
	virtual float GetSpeedMultiplier() const override 
	{
		return InnerStats ? InnerStats->GetSpeedMultiplier() : 1.0f;
	}
	virtual float GetMaxHealthBonus() const override 
	{
		return InnerStats ? InnerStats->GetMaxHealthBonus() : 0.0f;
	}
	virtual float GetHealthRestore() const override 
	{
		return InnerStats ? InnerStats->GetHealthRestore() : 0.0f;
	}
};
