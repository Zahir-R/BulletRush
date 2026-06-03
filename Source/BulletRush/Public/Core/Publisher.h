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
    void Subscribe(UObject* Subscriber);
    void Unsubscribe(UObject* Subscriber);

protected:
    UPROPERTY()
    TArray<TWeakObjectPtr<UObject>> Subscribers;
};