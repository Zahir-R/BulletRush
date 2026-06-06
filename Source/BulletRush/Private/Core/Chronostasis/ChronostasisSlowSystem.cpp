#include "Core/Chronostasis/ChronostasisSlowSystem.h"
#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Components/BuffComponent.h"
#include "Buffs/SlowDecorator.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UChronostasisSlowSystem::Initialize(AChronostasisFacade* Owner)
{
    OwnerFacade = Owner;
    SlowTriggerCount = 0;
}

void UChronostasisSlowSystem::Start()
{
    SlowTriggerCount = 0;
    StartTimer();
}

void UChronostasisSlowSystem::StartTimer()
{
    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().SetTimer(SlowTimerHandle, this, &UChronostasisSlowSystem::OnTimerExpired, 15.0f, false);
    }
}

void UChronostasisSlowSystem::OnTimerExpired()
{
    SlowTriggerCount++;

    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    APawn* PlayerPawn = World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
    if (PlayerPawn)
    {
        UBuffComponent* BuffComp = PlayerPawn->FindComponentByClass<UBuffComponent>();
        if (BuffComp)
        {
            BuffComp->ApplyBuff(USlowDecorator::StaticClass(), -1.0f, 0.0f);
        }
    }

    StartTimer();
    OnSlowTriggered.Broadcast();
}

void UChronostasisSlowSystem::Stop()
{
    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().ClearTimer(SlowTimerHandle);
    }
    SlowTriggerCount = 0;
}

void UChronostasisSlowSystem::ResetOnKill()
{
    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (World)
    {
        World->GetTimerManager().ClearTimer(SlowTimerHandle);
    }

    StartTimer();

    APawn* PlayerPawn = World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
    if (PlayerPawn)
    {
        UBuffComponent* BuffComp = PlayerPawn->FindComponentByClass<UBuffComponent>();
        if (BuffComp)
        {
            BuffComp->RemoveDecoratorByClass(USlowDecorator::StaticClass());
        }
    }
}
