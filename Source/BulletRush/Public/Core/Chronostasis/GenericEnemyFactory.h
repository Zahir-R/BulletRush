#pragma once

#include "CoreMinimal.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "GenericEnemyFactory.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UGenericEnemyFactory : public UChronostasisFactoryEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory")
	TSubclassOf<class AEnemyBase> EnemyClass;

	virtual AEnemyBase* CreateEnemy(UWorld* World, const FVector& Location) override;
};

template<typename T>
inline UGenericEnemyFactory* CreateGenericFactory(UObject* Outer)
{
	UGenericEnemyFactory* Factory = NewObject<UGenericEnemyFactory>(Outer);
	Factory->EnemyClass = T::StaticClass();
	return Factory;
}
