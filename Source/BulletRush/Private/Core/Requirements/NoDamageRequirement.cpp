#include "Core/Requirements/NoDamageRequirement.h"
#include "GameFramework/PlayerController.h"
#include "Player/PlayingPlayer.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

void UNoDamageRequirement::Initialize(APlayerController* Player, UWorld* World)
{
    bNoDamageTaken = true;
    if (!Player || !World) return;
    APawn* Pawn = Player->GetPawn();
    if (!Pawn) Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return;

    UHealthComponent* HealthComp = Pawn->FindComponentByClass<UHealthComponent>();
    if (HealthComp)
    {
        ObservedHealthComp = HealthComp;
        InitialHealth = HealthComp->CurrentHealth;
        // Subscribe to health changed dynamic delegate
        HealthComp->OnHealthChanged.AddDynamic(this, &UNoDamageRequirement::OnPlayerHealthChanged);
    }
}

void UNoDamageRequirement::Cleanup()
{
    if (ObservedHealthComp.IsValid())
    {
        ObservedHealthComp->OnHealthChanged.RemoveDynamic(this, &UNoDamageRequirement::OnPlayerHealthChanged);
    }
}

void UNoDamageRequirement::OnPlayerHealthChanged(float NewHealth)
{
    if (!ObservedHealthComp.IsValid()) return;
    if (NewHealth < InitialHealth)
    {
        bNoDamageTaken = false;
        // Unsubscribe since requirement already failed
        ObservedHealthComp->OnHealthChanged.RemoveDynamic(this, &UNoDamageRequirement::OnPlayerHealthChanged);
    }
}
