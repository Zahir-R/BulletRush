#include "Core/Requirements/NoPowerUpRequirement.h"
#include "Core/PowerUpUsagePublisher.h"
#include "Kismet/GameplayStatics.h"

void UNoPowerUpRequirement::Initialize(APlayerController* Player, UWorld* World)
{
    bNoPowerUpUsed = false;
    CachedPublisher = nullptr;
    if (!World) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, APowerUpUsagePublisher::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        APowerUpUsagePublisher* Pub = Cast<APowerUpUsagePublisher>(Found[0]);
        if (Pub)
        {
            CachedPublisher = Pub;
            Pub->Subscribe(this);
        }
    }
}

void UNoPowerUpRequirement::Cleanup()
{
    if (CachedPublisher.IsValid())
    {
        CachedPublisher->Unsubscribe(this);
        CachedPublisher = nullptr;
    }
}

void UNoPowerUpRequirement::Update(APublisher* Publisher)
{
    bNoPowerUpUsed = true;
}
