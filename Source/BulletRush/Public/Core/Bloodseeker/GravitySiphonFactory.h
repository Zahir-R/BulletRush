#pragma once

#include "CoreMinimal.h"
#include "Core/Bloodseeker/BloodseekerEnemyFactoryBase.h"

class BULLETRUSH_API UGravitySiphonFactory : public UBloodseekerEnemyFactoryBase
{
public:
	UClass* GravitySiphonClass = nullptr;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
