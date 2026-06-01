#pragma once

#include "CoreMinimal.h"
#include "Combat/Commands/BossCommand.h"
#include "MoveCommand.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UMoveCommand : public UBossCommand
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector TargetLocation;

	virtual void Execute(ASerXBoss* Boss) override;
};
