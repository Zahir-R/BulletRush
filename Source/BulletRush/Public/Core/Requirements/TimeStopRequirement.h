#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "TimeStopRequirement.generated.h"

// Observes ChronostasisFacade's time stop events
UCLASS(Blueprintable)
class BULLETRUSH_API UTimeStopRequirement : public UObject, public ILevelRequirement
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override;
    virtual bool IsCompleted() const override { return CurrentStops >= RequiredStops; }
    virtual FString GetDescription() const override { return FString::Printf(TEXT("Trigger time stop %d times"), RequiredStops); }
    virtual void Cleanup() override;

    UPROPERTY(EditAnywhere)
    int32 RequiredStops = 3;

    UFUNCTION()
    void RegisterTimeStop();

protected:
    int32 CurrentStops = 0;
    TWeakObjectPtr<class AChronostasisFacade> ObservedFacade;
};
