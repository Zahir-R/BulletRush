#pragma once

#include "CoreMinimal.h"
#include "Core/Bloodseeker/BloodseekerEnemyFactoryBase.h"
#include "GravitySiphonFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UGravitySiphonFactory : public UBloodseekerEnemyFactoryBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> GravitySiphonClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
