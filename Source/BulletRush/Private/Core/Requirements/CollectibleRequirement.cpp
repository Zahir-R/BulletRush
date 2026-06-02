#include "Core/Requirements/CollectibleRequirement.h"
#include "Core/CollectiblePickupPublisher.h"
#include "Kismet/GameplayStatics.h"

void UCollectibleRequirement::Initialize(APlayerController* Player, UWorld* World)
{
    PickedCount = 0;
    CachedPublisher = nullptr;
    if (!World) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, ACollectiblePickupPublisher::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        ACollectiblePickupPublisher* Pub = Cast<ACollectiblePickupPublisher>(Found[0]);
        if (Pub)
        {
            CachedPublisher = Pub;
            Pub->Subscribe(this);
        }
    }
}

void UCollectibleRequirement::Cleanup()
{
    if (CachedPublisher.IsValid())
    {
        CachedPublisher->Unsubscribe(this);
        CachedPublisher = nullptr;
    }
}

void UCollectibleRequirement::Update(APublisher* Publisher)
{
    PickedCount++;
}
