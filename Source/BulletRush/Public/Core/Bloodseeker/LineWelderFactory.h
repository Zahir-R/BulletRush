#pragma once

#include "CoreMinimal.h"
#include "Core/Bloodseeker/BloodseekerEnemyFactoryBase.h"

class BULLETRUSH_API ULineWelderFactory : public UBloodseekerEnemyFactoryBase
{
public:
	UClass* LineWelderClass = nullptr;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
