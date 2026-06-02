#pragma once

#include "CoreMinimal.h"
#include "Core/Bloodseeker/BloodseekerEnemyFactoryBase.h"
#include "KamikazeFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UKamikazeFactory : public UBloodseekerEnemyFactoryBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> KamikazeClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
