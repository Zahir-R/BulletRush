#pragma once

#include "CoreMinimal.h"
#include "Combat/Commands/BossCommand.h"
#include "SpawnLinkerCommand.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API USpawnLinkerCommand : public UBossCommand
{
	GENERATED_BODY()

public:
	virtual void Execute(ASerXBoss* Boss) override;
};
