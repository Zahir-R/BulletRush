#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "NoDamageRequirement.generated.h"

class UHealthComponent;

// Observer pattern: subscribes to player's health changes
UCLASS(Blueprintable)
class BULLETRUSH_API UNoDamageRequirement : public UObject, public ILevelRequirement
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override;
    virtual bool IsCompleted() const override { return bNoDamageTaken; }
    virtual FString GetDescription() const override { return FString("Do not take any damage"); }
    virtual void Cleanup() override;

protected:
    // Handler for health change events
    UFUNCTION()
    void OnPlayerHealthChanged(float NewHealth);

    // Track initial and current state
    bool bNoDamageTaken = true;
    float InitialHealth = 0.f;
    TWeakObjectPtr<UHealthComponent> ObservedHealthComp;
};
