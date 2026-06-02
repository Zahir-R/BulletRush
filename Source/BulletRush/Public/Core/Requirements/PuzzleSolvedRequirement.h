#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "Core/Subscriber.h"
#include "PuzzleSolvedRequirement.generated.h"

class APuzzleEventPublisher;

UCLASS(Blueprintable)
class BULLETRUSH_API UPuzzleSolvedRequirement : public UObject, public ILevelRequirement, public ISubscriber
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override;
    virtual bool IsCompleted() const override { return bSolved; }
    virtual FString GetDescription() const override { return FString("Solve the puzzle"); }
    virtual void Cleanup() override;

    virtual void Update(class APublisher* Publisher) override;

protected:
    bool bSolved = false;
    TWeakObjectPtr<APuzzleEventPublisher> CachedPublisher;
};
