#pragma once

#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "PuzzleEventPublisher.generated.h"

UCLASS()
class BULLETRUSH_API APuzzleEventPublisher : public APublisher
{
	GENERATED_BODY()

public:
	void SetSolved(bool bInSolved);
	bool IsSolved() const { return bSolved; }

private:
	bool bSolved = false;
};
