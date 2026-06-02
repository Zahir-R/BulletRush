#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BloodseekerEnemyFactoryBase.generated.h"

class AEnemyBase;

UCLASS(Abstract, Blueprintable, BlueprintType)
class BULLETRUSH_API UBloodseekerEnemyFactoryBase : public UObject
{
	GENERATED_BODY()

public:
	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) PURE_VIRTUAL(UBloodseekerEnemyFactoryBase::CreateEnemy, return nullptr;);
};
