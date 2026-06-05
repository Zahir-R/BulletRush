#include "Core/Publisher.h"
#include "Core/Subscriber.h"

APublisher::APublisher()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APublisher::Subscribe(UObject* Subscriber)
{
    if (Subscriber && IsValid(Subscriber))
        Subscribers.AddUnique(TWeakObjectPtr<UObject>(Subscriber));
}

void APublisher::Unsubscribe(UObject* Subscriber)
{
    Subscribers.RemoveAll([Subscriber](const TWeakObjectPtr<UObject>& WP)
        {
            return !WP.IsValid() || WP.Get() == Subscriber;
        });
}

void APublisher::NotifySubscribers()
{
    for (int32 i = Subscribers.Num() - 1; i >= 0; --i)
    {
        if (!Subscribers[i].IsValid())
        {
            Subscribers.RemoveAt(i);
            continue;
        }

        UObject* Obj = Subscribers[i].Get();
        ISubscriber* Sub = Cast<ISubscriber>(Obj);
        if (Sub)
            Sub->Update(this);
    }
}