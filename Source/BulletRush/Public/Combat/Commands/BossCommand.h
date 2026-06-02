#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BossCommand.generated.h"

class ASerXBoss;

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class BULLETRUSH_API UBossCommand : public UObject
{
	GENERATED_BODY()

public:
	float Timestamp = 0.f;

	virtual void Execute(ASerXBoss* Boss) PURE_VIRTUAL(UBossCommand::Execute, );
};
