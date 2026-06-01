#pragma once

#include "CoreMinimal.h"
#include "Combat/Commands/BossCommand.h"
#include "ActivateZoneCommand.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UActivateZoneCommand : public UBossCommand
{
	GENERATED_BODY()

public:
	virtual void Execute(ASerXBoss* Boss) override;
};
