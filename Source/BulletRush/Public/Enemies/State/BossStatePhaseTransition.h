#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "BossStatePhaseTransition.generated.h"

class ABossBase;

UCLASS(Blueprintable)
class BULLETRUSH_API UBossStatePhaseTransition : public UBossState
{
	GENERATED_BODY()

public:
	UBossStatePhaseTransition();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};
