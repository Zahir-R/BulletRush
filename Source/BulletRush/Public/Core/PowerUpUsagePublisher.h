#pragma once

#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "PowerUpUsagePublisher.generated.h"

UCLASS()
class BULLETRUSH_API APowerUpUsagePublisher : public APublisher
{
	GENERATED_BODY()

public:
	void MarkPowerUpUsed();
};
