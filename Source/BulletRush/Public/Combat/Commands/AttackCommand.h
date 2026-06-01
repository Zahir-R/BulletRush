#pragma once

#include "CoreMinimal.h"
#include "Combat/Commands/BossCommand.h"
#include "AttackCommand.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UAttackCommand : public UBossCommand
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 AttackPatternIndex;

	virtual void Execute(ASerXBoss* Boss) override;
};
