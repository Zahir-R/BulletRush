#include "Core/Chronostasis/ChronostasisSecretManager.h"
#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Core/BulletRushHUD.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UChronostasisSecretManager::Initialize(AChronostasisFacade* Owner)
{
    OwnerFacade = Owner;
}

void UChronostasisSecretManager::Start()
{
    bIsActive = true;
    TimeRemaining = 120.f;
    bTimerPaused = false;

    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UChronostasisSecretManager::TickTimer, 1.f, true);
    }
}

void UChronostasisSecretManager::TickTimer()
{
    if (bTimerPaused) return;
    TimeRemaining -= 1.f;

    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
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

    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (World)
    {
        UProjectilesSubsystem* ProjSys = World->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
        if (ProjSys) ProjSys->SetSecretLevel(false);
    }

    OnSecretTimeUp.Broadcast();
}

void UChronostasisSecretManager::ClearAllTimers()
{
    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
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
