#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "Core/Subscriber.h"
#include "CollectibleRequirement.generated.h"

class ACollectiblePickupPublisher;

UCLASS(Blueprintable)
class BULLETRUSH_API UCollectibleRequirement : public UObject, public ILevelRequirement, public ISubscriber
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override;
    virtual bool IsCompleted() const override { return PickedCount >= RequiredCount; }
    virtual FString GetDescription() const override { return FString::Printf(TEXT("Collect %d items"), RequiredCount); }
    virtual void Cleanup() override;

    virtual void Update(class APublisher* Publisher) override;

    UPROPERTY(EditAnywhere)
    int32 RequiredCount = 10;

protected:
    int32 PickedCount = 0;
    TWeakObjectPtr<ACollectiblePickupPublisher> CachedPublisher;
};
