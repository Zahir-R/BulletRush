#pragma once

#include "CoreMinimal.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "ExpansiveFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UExpansiveFactory : public UChronostasisFactoryEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> ExpansiveClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
