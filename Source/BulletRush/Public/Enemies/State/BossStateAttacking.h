#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "BossStateAttacking.generated.h"

class ABossBase;

UCLASS(Blueprintable)
class BULLETRUSH_API UBossStateAttacking : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateAttacking();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void UpdateState(ABossBase* Boss, float DeltaTime) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};
