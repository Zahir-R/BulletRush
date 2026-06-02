#pragma once

#include "CoreMinimal.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "LinkerFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API ULinkerFactory : public UChronostasisFactoryEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> LinkerClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
