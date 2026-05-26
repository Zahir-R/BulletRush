#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "CollectibleRequirement.generated.h"

// Observer: external systems call RegisterPick when collectibles are picked
UCLASS(Blueprintable)
class BULLETRUSH_API UCollectibleRequirement : public UObject, public ILevelRequirement
{
    GENERATED_BODY()
public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override { /* nothing to subscribe by default */ }
    virtual bool IsCompleted() const override { return PickedCount >= RequiredCount; }
    virtual FString GetDescription() const override { return FString::Printf(TEXT("Collect %d items"), RequiredCount); }
    virtual void Cleanup() override { }

    UPROPERTY(EditAnywhere)
    int32 RequiredCount = 10;

    UFUNCTION()
    void RegisterPick()
    {
        PickedCount++;
    }

    int32 PickedCount = 0;
};
