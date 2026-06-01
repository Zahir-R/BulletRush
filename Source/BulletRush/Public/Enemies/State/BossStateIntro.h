#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "BossStateIntro.generated.h"

class ABossBase;

UCLASS(Blueprintable)
class BULLETRUSH_API UBossStateIntro : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateIntro();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};
