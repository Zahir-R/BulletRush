#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "Core/Subscriber.h"
#include "NoPowerUpRequirement.generated.h"

class APowerUpUsagePublisher;

UCLASS(Blueprintable)
class BULLETRUSH_API UNoPowerUpRequirement : public UObject, public ILevelRequirement, public ISubscriber
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override;
    virtual bool IsCompleted() const override { return !bNoPowerUpUsed; }
    virtual FString GetDescription() const override { return FString("Do not use power-ups"); }
    virtual void Cleanup() override;

    virtual void Update(class APublisher* Publisher) override;

protected:
    bool bNoPowerUpUsed = false;
    TWeakObjectPtr<APowerUpUsagePublisher> CachedPublisher;
};
