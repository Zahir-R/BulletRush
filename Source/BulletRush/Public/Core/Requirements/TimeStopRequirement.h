#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "Core/Subscriber.h"
#include "TimeStopRequirement.generated.h"

class AChronostasisFacade;

UCLASS(Blueprintable)
class BULLETRUSH_API UTimeStopRequirement : public UObject, public ILevelRequirement, public ISubscriber
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override;
    virtual bool IsCompleted() const override { return CurrentStops >= RequiredStops; }
    virtual FString GetDescription() const override { return FString::Printf(TEXT("Trigger time stop %d times"), RequiredStops); }
    virtual void Cleanup() override;

    virtual void Update(class APublisher* Publisher) override;

    UPROPERTY(EditAnywhere)
    int32 RequiredStops = 3;

protected:
    int32 CurrentStops = 0;
    TWeakObjectPtr<AChronostasisFacade> CachedPublisher;
};
