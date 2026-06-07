#pragma once

#include "CoreMinimal.h"

class AEnemyBase;

class BULLETRUSH_API UBloodseekerEnemyFactoryBase
{
public:
	virtual ~UBloodseekerEnemyFactoryBase() {}
	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) = 0;
};
