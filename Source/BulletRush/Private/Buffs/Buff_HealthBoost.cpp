#include "Buffs/Buff_HealthBoost.h"
#include "Components/HealthComponent.h"

void UBuff_HealthBoost::Apply(AActor* InOwner)
{
	Super::Apply(InOwner);
	if (UHealthComponent* HealthComp = Owner->FindComponentByClass<UHealthComponent>())
	{
		HealthComp->MaxHealth += ExtraHealth;
		HealthComp->Heal(ExtraHealth);
	}
}

void UBuff_HealthBoost::Remove()
{
	if (UHealthComponent* HealthComp = Owner->FindComponentByClass<UHealthComponent>())
	{
		HealthComp->MaxHealth -= ExtraHealth;
		if (HealthComp->CurrentHealth > HealthComp->MaxHealth) HealthComp->CurrentHealth = HealthComp->MaxHealth;
		
	}
	Super::Remove();
}