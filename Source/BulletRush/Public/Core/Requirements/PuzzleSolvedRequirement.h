#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ILevelRequirement.h"
#include "PuzzleSolvedRequirement.generated.h"

// Simple observer: external systems call SetSolved when puzzle is completed
UCLASS(Blueprintable)
class BULLETRUSH_API UPuzzleSolvedRequirement : public UObject, public ILevelRequirement
{
    GENERATED_BODY()

public:
    virtual void Initialize(APlayerController* Player, UWorld* World) override { bSolved = false; }
    virtual bool IsCompleted() const override { return bSolved; }
    virtual FString GetDescription() const override { return FString("Solve the puzzle"); }
    virtual void Cleanup() override { }

    UFUNCTION()
    void SetSolved(bool Solved)
    {
        bSolved = Solved;
    }

protected:
    bool bSolved = false;
};
