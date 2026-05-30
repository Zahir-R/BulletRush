#pragma once

#include "CoreMinimal.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "ChargerFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UChargerFactory : public UChronostasisFactoryEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> ChargerClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
