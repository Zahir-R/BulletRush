#include "../../Public/Components/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Engine/World.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth);
	bDead = false;
	bIsInvulnerable = false;
}

float UHealthComponent::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvulnerable || bDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	float DamageApplied = FMath::Min(DamageAmount, CurrentHealth);
	CurrentHealth -= DamageApplied;

	OnHealthChanged.Broadcast(CurrentHealth);
	if (CurrentHealth <= 0.0f)
	{
		bDead = true;
		OnDeath.Broadcast();
	}

	UE_LOG(LogTemp, Warning, TEXT("Damage taken: %f, Current Health: %f"), DamageApplied, CurrentHealth);
	return DamageApplied;
}

void UHealthComponent::Heal(float Amount)
{
	if (bDead || Amount <= 0.0f)
	{
		return;
	}
	CurrentHealth = FMath::Min(CurrentHealth + Amount, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth);
	UE_LOG(LogTemp, Warning, TEXT("Healed: %f, Current Health: %f"), Amount, CurrentHealth);
}

void UHealthComponent::SetInvulnerable(bool bInvulnerable, float Duration)
{
	bIsInvulnerable = bInvulnerable;
	if (bInvulnerable && Duration > 0.0f)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(InvulnerabilityTimer);
			World->GetTimerManager().SetTimer(InvulnerabilityTimer, [this]()
			{
				bIsInvulnerable = false;
				}, Duration, false);
		}
	}
	else if (!bInvulnerable)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(InvulnerabilityTimer);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Invulnerability set to: %s for duration: %f"), bInvulnerable ? TEXT("true") : TEXT("false"), Duration);
}