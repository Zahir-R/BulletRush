#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Subscriber.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USubscriber : public UInterface
{
	GENERATED_BODY()
};

class BULLETRUSH_API ISubscriber
{
	GENERATED_BODY()

public:
	virtual void Update(class APublisher* Publisher) = 0;
};
