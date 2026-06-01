#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "BossStateIdle.generated.h"

class ABossBase;

UCLASS(Blueprintable)
class BULLETRUSH_API UBossStateIdle : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateIdle();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void UpdateState(ABossBase* Boss, float DeltaTime) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};
