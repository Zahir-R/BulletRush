#include "Enemies/Chronostasis/ChronostasisExpansive.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/BuffComponent.h"
#include "Buffs/PulseEffectDecorator.h"

AChronostasisExpansive::AChronostasisExpansive()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AChronostasisExpansive::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &AChronostasisExpansive::TriggerPulse, 10.0f, true);
}

void AChronostasisExpansive::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(PulseTimerHandle);
    Super::EndPlay(EndPlayReason);
}

void AChronostasisExpansive::TriggerPulse()
{
    // Encuentra y aplica el buff a Drones
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChronostasisDrone::StaticClass(), Found);
    for (AActor* A : Found)
    {
        if (AChronostasisDrone* Drone = Cast<AChronostasisDrone>(A))
        {
            // Let the Drone manage restoration; apply a shorter, moderate buff
            Drone->ApplySpeedBuff(2.0f, 2.0f, 1.5f);
        }
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UBuffComponent* BuffComp = PlayerPawn->FindComponentByClass<UBuffComponent>();
        if (BuffComp)
        {
            BuffComp->ApplyBuff(UPulseEffectDecorator::StaticClass(), 2.0f, 0.0f);
        }
    }
}
