#pragma once

#include "CoreMinimal.h"
#include "Combat/Commands/BossCommand.h"
#include "SpawnChargerCommand.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API USpawnChargerCommand : public UBossCommand
{
	GENERATED_BODY()

public:
	virtual void Execute(ASerXBoss* Boss) override;
};
