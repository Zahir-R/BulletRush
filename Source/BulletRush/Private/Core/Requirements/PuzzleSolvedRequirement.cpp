#include "Core/Requirements/PuzzleSolvedRequirement.h"
#include "Core/PuzzleEventPublisher.h"
#include "Kismet/GameplayStatics.h"

void UPuzzleSolvedRequirement::Initialize(APlayerController* Player, UWorld* World)
{
    bSolved = false;
    CachedPublisher = nullptr;
    if (!World) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, APuzzleEventPublisher::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        APuzzleEventPublisher* Pub = Cast<APuzzleEventPublisher>(Found[0]);
        if (Pub)
        {
            CachedPublisher = Pub;
            Pub->Subscribe(this);
        }
    }
}

void UPuzzleSolvedRequirement::Cleanup()
{
    if (CachedPublisher.IsValid())
    {
        CachedPublisher->Unsubscribe(this);
        CachedPublisher = nullptr;
    }
}

void UPuzzleSolvedRequirement::Update(APublisher* Publisher)
{
    APuzzleEventPublisher* PPub = Cast<APuzzleEventPublisher>(Publisher);
    if (PPub)
    {
        bSolved = PPub->IsSolved();
    }
}
