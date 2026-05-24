#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	float MaxHealth = 100.0f;
	float CurrentHealth;

	FOnHealthChanged OnHealthChanged;
	FOnDeath OnDeath;

	UFUNCTION(BlueprintCallable) float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	UFUNCTION(BlueprintCallable) void Heal(float Amount);
	UFUNCTION(BlueprintCallable) void SetInvulnerable(bool bInvulnerable, float Duration = 0.0f);

	bool IsDead() const { return bDead; }
	bool IsInvulnerable() const { return bIsInvulnerable; }

public:
	virtual void BeginPlay() override;
	bool bDead = false;
	bool bIsInvulnerable = false;
	FTimerHandle InvulnerabilityTimer;
};
