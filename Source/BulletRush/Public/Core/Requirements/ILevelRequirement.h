#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ILevelRequirement.generated.h"

UINTERFACE(MinimalAPI)
class ULevelRequirement : public UInterface
{
    GENERATED_BODY()
};

class BULLETRUSH_API ILevelRequirement
{
    GENERATED_BODY()
public:
    virtual void Initialize(APlayerController* Player, UWorld* World) = 0;
    virtual bool IsCompleted() const = 0;
    virtual FString GetDescription() const = 0;
    virtual void Cleanup() = 0;
};
