#pragma once

#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "CollectiblePickupPublisher.generated.h"

UCLASS()
class BULLETRUSH_API ACollectiblePickupPublisher : public APublisher
{
	GENERATED_BODY()

public:
	void RegisterPick();
};
