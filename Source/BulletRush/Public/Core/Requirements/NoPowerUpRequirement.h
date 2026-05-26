#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "NoPowerUpRequirement.generated.h"

// Observer: simple requirement. Designer or powerup actors should call MarkPowerUpUsed when applied.
UCLASS(Blueprintable)
class BULLETRUSH_API UNoPowerUpRequirement : public UObject, public ILevelRequirement
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override { bNoPowerUpUsed = false; }
    virtual bool IsCompleted() const override { return !bNoPowerUpUsed; }
    virtual FString GetDescription() const override { return FString("Do not use power-ups"); }
    virtual void Cleanup() override { }

    UFUNCTION()
    void MarkPowerUpUsed()
    {
        bNoPowerUpUsed = true;
    }

protected:
    bool bNoPowerUpUsed = false;
};
