#pragma once

#include "CoreMinimal.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "DroneFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UDroneFactory : public UChronostasisFactoryEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> DroneClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};
