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
            Drone->ApplySpeedBuff(5.0f, 5.0f, 5.0f);
            FTimerHandle LocalHandle;
            FTimerDelegate Del = FTimerDelegate::CreateLambda([Drone]() {
                if (Drone && !Drone->IsPendingKill())
                {
                    Drone->RemoveSpeedBuff();
                }
            });
            Drone->GetWorldTimerManager().SetTimer(LocalHandle, Del, 2.0f, false);
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
