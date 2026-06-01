#pragma once

#include "CoreMinimal.h"
#include "BossStateBase.generated.h"

class ABossBase;

class ABossBase;

UCLASS(Abstract, BlueprintType)
class BULLETRUSH_API UBossState : public UObject
{
	GENERATED_BODY()

public:
	UBossState();

	virtual void EnterState(ABossBase* Boss);
	virtual void UpdateState(ABossBase* Boss, float DeltaTime);
	virtual void ExitState(ABossBase* Boss);

	UFUNCTION(BlueprintCallable, Category = "State")
	virtual FName GetStateTagName() const;
};
