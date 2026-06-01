#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BloodseekerEnemyFactory.generated.h"

class AEnemyBase;
class AKamikazeEnemy;
class ALineWelderEnemy;
class AGravitySiphonEnemy;

UCLASS(Blueprintable)
class BULLETRUSH_API UBloodseekerEnemyFactory : public UObject
{
    GENERATED_BODY()

public:
    UBloodseekerEnemyFactory();

    UFUNCTION(BlueprintCallable, Category = "Factory")
    AKamikazeEnemy* CreateKamikaze(UWorld* World, const FVector& SpawnLocation);

    UFUNCTION(BlueprintCallable, Category = "Factory")
    ALineWelderEnemy* CreateLineWelder(UWorld* World, const FVector& SpawnLocation);

    UFUNCTION(BlueprintCallable, Category = "Factory")
    AGravitySiphonEnemy* CreateGravitySiphon(UWorld* World, const FVector& SpawnLocation);

protected:
    UPROPERTY(EditAnywhere, Category = "Classes")
    TSubclassOf<AKamikazeEnemy> KamikazeClass;

    UPROPERTY(EditAnywhere, Category = "Classes")
    TSubclassOf<ALineWelderEnemy> LineWelderClass;

    UPROPERTY(EditAnywhere, Category = "Classes")
    TSubclassOf<AGravitySiphonEnemy> GravitySiphonClass;
};
