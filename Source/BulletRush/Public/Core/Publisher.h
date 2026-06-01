#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Subscriber.h"
#include "Publisher.generated.h"

UCLASS()
class BULLETRUSH_API APublisher : public AActor
{
	GENERATED_BODY()

public:
	APublisher();

	void NotifySubscribers();
	void Subscribe(TScriptInterface<ISubscriber> Subscriber);
	void Unsubscribe(TScriptInterface<ISubscriber> Subscriber);

protected:
	UPROPERTY()
	TArray<TScriptInterface<ISubscriber>> Subscribers;
};
