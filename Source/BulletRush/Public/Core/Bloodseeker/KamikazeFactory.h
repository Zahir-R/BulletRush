#pragma once

#include "CoreMinimal.h"
#include "Core/Bloodseeker/BloodseekerEnemyFactoryBase.h"

class BULLETRUSH_API UKamikazeFactory : public UBloodseekerEnemyFactoryBase
{
public:
	UClass* KamikazeClass = nullptr;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
