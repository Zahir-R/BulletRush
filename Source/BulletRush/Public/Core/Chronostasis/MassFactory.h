#pragma once

#include "CoreMinimal.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "MassFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UMassFactory : public UChronostasisFactoryEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> MassClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
