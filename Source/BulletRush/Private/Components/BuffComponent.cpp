#include "Components/BuffComponent.h"
#include "Player/PlayingPlayer.h"
#include "Buffs/DoubleDamage.h"
#include "Buffs/SpeedBoost.h"
#include "Buffs/HealthBonus.h"
#include "Engine/World.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuffComponent::ApplyBuff(TSubclassOf<UPlayerStatsDecorator> DecoratorClass, float Duration, float Magnitude)
{
	APlayingPlayer* Player = Cast<APlayingPlayer>(GetOwner());
	if (!Player || !DecoratorClass) return;

	UPlayerStatsDecorator* NewDecorator = NewObject<UPlayerStatsDecorator>(this, DecoratorClass);

	if (USpeedBoost* Speed = Cast<USpeedBoost>(NewDecorator)) Speed->SetMultiplier(Magnitude);
	else if (UHealthBonus* Health = Cast<UHealthBonus>(NewDecorator)) Health->SetExtraHealth(Magnitude);

	Player->WrapStats(NewDecorator);

	FActiveDecorator Active;
	Active.Decorator = NewDecorator;
	GetWorld()->GetTimerManager().SetTimer(Active.Timer, [this, NewDecorator]()
		{
			RemoveDecorator(NewDecorator);
		}, Duration, false);
	ActiveDecorators.Add(Active);
}

void UBuffComponent::RemoveDecorator(UPlayerStatsDecorator* Decorator)
{
	APlayingPlayer* Player = Cast<APlayingPlayer>(GetOwner());
	if (!Player || !Decorator) return;

	if (Player->GetCurrentStats().GetObject() == Decorator) Player->UnwrapStats();
	else
	{
		TScriptInterface<IPlayerStatsInterface> CurrStats = Player->GetCurrentStats();
		UPlayerStatsDecorator* Dec = Cast<UPlayerStatsDecorator>(CurrStats.GetObject());

		while (Dec && Dec->GetInnerStats().GetObject())
		{
			if (Dec->GetInnerStats().GetObject() == Decorator)
			{
				Dec->SetInner(Decorator->GetInnerStats());
				Player->RefreshStatsFromChain();
				break;
			}
			Dec = Cast<UPlayerStatsDecorator>(Dec->GetInnerStats().GetObject());
		}
	}
	for (int32 i = 0; i < ActiveDecorators.Num(); ++i)
	{
		if (ActiveDecorators[i].Decorator == Decorator)
		{
			GetWorld()->GetTimerManager().ClearTimer(ActiveDecorators[i].Timer);
			ActiveDecorators.RemoveAt(i);
			break;
		}
	}
}

void UBuffComponent::ClearAllBuffs()
{
	while (ActiveDecorators.Num() > 0)
		RemoveDecorator(ActiveDecorators[0].Decorator);
}