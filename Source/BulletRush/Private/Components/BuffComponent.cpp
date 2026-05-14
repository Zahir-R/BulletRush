#include "Components/BuffComponent.h"
#include "Buffs/BuffBase.h"
#include "Buffs/Buff_HealthBoost.h"
#include "Buffs/Buff_SpeedBoost.h"
#include "Buffs/Buff_DoubleDamage.h"
#include "Engine/World.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuffComponent::ApplyBuff(TSubclassOf<UBuffBase> BuffClass, float Duration, float Magnitude)
{
	if (!BuffClass || !GetOwner()) return;
	
	UBuffBase* NewBuff = NewObject<UBuffBase>(this, BuffClass);
	NewBuff->Duration = Duration;

	if (UBuff_HealthBoost* HealthBuff = Cast<UBuff_HealthBoost>(NewBuff)) HealthBuff->ExtraHealth = Magnitude;
	else if (UBuff_SpeedBoost* SpeedBuff = Cast<UBuff_SpeedBoost>(NewBuff)) SpeedBuff->SpeedMultiplier = Magnitude;
	//else if (UBuff_DoubleDamage* DamageBuff = Cast<UBuff_DoubleDamage>(NewBuff)) DamageBuff->DamageMultiplier = Magnitude;

	NewBuff->Apply(GetOwner());
	NewBuff->StartDurationTimer();
	ActiveBuffs.Add(NewBuff);
	
}

void UBuffComponent::ClearAllBuffs()
{
	for (UBuffBase* Buff : ActiveBuffs)
	{
		if (Buff) Buff->Remove();
	}
	ActiveBuffs.Empty();
}