#include "Core/Chronostasis/ChronostasisSecretManager.h"
#include "Core/BulletRushHUD.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UChronostasisSecretManager::Initialize(AActor* Owner)
{
    OwnerActor = Owner;
}

void UChronostasisSecretManager::Start()
{
    bIsActive = true;
    TimeRemaining = 120.f;
    bTimerPaused = false;

    UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UChronostasisSecretManager::TickTimer, 1.f, true);
    }
}

void UChronostasisSecretManager::TickTimer()
{
    if (bTimerPaused) return;
    TimeRemaining -= 1.f;

    UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
    APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
    ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
    if (HUD)
    {
        HUD->SetCountdown(TimeRemaining);
    }

    if (TimeRemaining <= 0.f)
    {
        if (World)
        {
            World->GetTimerManager().ClearTimer(CountdownTimerHandle);
        }
        bTimerPaused = true;

        if (HUD)
        {
            HUD->ShowMessage("TIME'S UP! Teleporting...", 3.f);
        }

        if (World)
        {
            World->GetTimerManager().SetTimer(TeleportDelayHandle, this, &UChronostasisSecretManager::OnTimeUpTeleport, 3.f, false);
        }
    }
}

void UChronostasisSecretManager::OnTimeUpTeleport()
{
    bIsActive = false;

    // Unused — ChronostasisSecretFacade manages its own timer
    OnSecretTimeUp.Broadcast();
}

void UChronostasisSecretManager::ClearAllTimers()
{
    UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().ClearTimer(CountdownTimerHandle);
        World->GetTimerManager().ClearTimer(TeleportDelayHandle);
    }
    bTimerPaused = false;
}

void UChronostasisSecretManager::Reset()
{
    ClearAllTimers();
    bIsActive = false;
    TimeRemaining = 120.f;
}
