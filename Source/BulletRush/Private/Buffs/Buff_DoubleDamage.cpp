#include "Buffs/Buff_DoubleDamage.h"
#include "Components/WeaponBaseComponent.h"
#include "Player/PlayingPlayer.h"

void UBuff_DoubleDamage::Apply(AActor* InOwner)
{
	Super::Apply(InOwner);
	APlayingPlayer* Player = Cast<APlayingPlayer>(InOwner);
	if (Player)
	{
		for (UWeaponBaseComponent* Weapon : Player->EquippedWeapons)
		{
			UE_LOG(LogTemp, Warning, TEXT("Applying double damage buff to player's bullets"));
		}
	}
}

void UBuff_DoubleDamage::Remove()
{
	Super::Remove();
	APlayingPlayer* Player = Cast<APlayingPlayer>(Owner);
	if (Player)
	{
		for (UWeaponBaseComponent* Weapon : Player->EquippedWeapons)
		{
			UE_LOG(LogTemp, Warning, TEXT("Removing double damage buff from player's bullets"));
		}
	}
}