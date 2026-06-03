#pragma once

#include "CoreMinimal.h"
#include "Core/Bloodseeker/BloodseekerEnemyFactoryBase.h"
#include "LineWelderFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API ULineWelderFactory : public UBloodseekerEnemyFactoryBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> LineWelderClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
