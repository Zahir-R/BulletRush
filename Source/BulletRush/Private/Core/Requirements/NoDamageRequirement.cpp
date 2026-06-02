#include "Core/Requirements/NoDamageRequirement.h"
#include "Core/PlayerHealthPublisher.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UNoDamageRequirement::Initialize(APlayerController* Player, UWorld* World)
{
    bNoDamageTaken = true;
    CachedPublisher = nullptr;
    if (!World) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, APlayerHealthPublisher::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        APlayerHealthPublisher* Pub = Cast<APlayerHealthPublisher>(Found[0]);
        if (Pub)
        {
            CachedPublisher = Pub;
            Pub->Subscribe(this);
        }
    }
}

void UNoDamageRequirement::Cleanup()
{
    if (CachedPublisher.IsValid())
    {
        CachedPublisher->Unsubscribe(this);
        CachedPublisher = nullptr;
    }
}

void UNoDamageRequirement::Update(APublisher* Publisher)
{
    APlayerHealthPublisher* HPub = Cast<APlayerHealthPublisher>(Publisher);
    if (HPub && HPub->GetCurrentHealth() < HPub->GetInitialHealth())
    {
        bNoDamageTaken = false;
        HPub->Unsubscribe(this);
    }
}
