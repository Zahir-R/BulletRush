#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "BossStateDead.generated.h"

class ABossBase;

UCLASS(Blueprintable)
class BULLETRUSH_API UBossStateDead : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateDead();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};
