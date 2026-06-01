#include "Core/Publisher.h"

APublisher::APublisher()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APublisher::Subscribe(TScriptInterface<ISubscriber> Subscriber)
{
	if (Subscriber.GetObject())
	{
		Subscribers.AddUnique(Subscriber);
	}
}

void APublisher::Unsubscribe(TScriptInterface<ISubscriber> Subscriber)
{
	Subscribers.Remove(Subscriber);
}

void APublisher::NotifySubscribers()
{
	for (int32 i = Subscribers.Num() - 1; i >= 0; --i)
	{
		if (Subscribers[i].GetObject())
		{
			Subscribers[i]->Update(this);
		}
		else
		{
			Subscribers.RemoveAt(i);
		}
	}
}
