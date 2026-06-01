#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "BossStateStunned.generated.h"

class ABossBase;

UCLASS(Blueprintable)
class BULLETRUSH_API UBossStateStunned : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateStunned();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};
