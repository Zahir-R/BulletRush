#pragma once
#include "CoreMinimal.h"
#include "WaveConfig.generated.h"

USTRUCT(BlueprintType)
struct FWaveConfig
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere)
    int32 DroneCount = 0;
    UPROPERTY(EditAnywhere)
    int32 MassCount = 0;
    UPROPERTY(EditAnywhere)
    int32 ExpansiveCount = 0;
    UPROPERTY(EditAnywhere)
    int32 ChargerCount = 0;
    UPROPERTY(EditAnywhere)
    int32 LinkerCount = 0;
    UPROPERTY(EditAnywhere)
    TArray<FVector> SpawnPoints;
};
