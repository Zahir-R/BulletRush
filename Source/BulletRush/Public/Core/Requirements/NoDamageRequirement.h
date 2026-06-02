#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "Core/Subscriber.h"
#include "NoDamageRequirement.generated.h"

class APlayerHealthPublisher;

UCLASS(Blueprintable)
class BULLETRUSH_API UNoDamageRequirement : public UObject, public ILevelRequirement, public ISubscriber
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override;
    virtual bool IsCompleted() const override { return bNoDamageTaken; }
    virtual FString GetDescription() const override { return FString("Do not take any damage"); }
    virtual void Cleanup() override;

    virtual void Update(class APublisher* Publisher) override;

protected:
    bool bNoDamageTaken = true;
    TWeakObjectPtr<APlayerHealthPublisher> CachedPublisher;
};
