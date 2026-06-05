#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "BossStateUltimate.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UBossStateUltimate : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateUltimate();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void UpdateState(ABossBase* Boss, float DeltaTime) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;

private:
	FTimerHandle Wave2Timer;
	FTimerHandle Wave3Timer;
	FTimerHandle DurationTimer;
};
