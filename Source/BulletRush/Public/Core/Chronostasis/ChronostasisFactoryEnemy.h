#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ChronostasisFactoryEnemy.generated.h"

class AEnemyBase;

UCLASS(Abstract, Blueprintable, BlueprintType)
class BULLETRUSH_API UChronostasisFactoryEnemy : public UObject
{
	GENERATED_BODY()

public:
	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) PURE_VIRTUAL(UChronostasisFactoryEnemy::CreateEnemy, return nullptr;);
};
