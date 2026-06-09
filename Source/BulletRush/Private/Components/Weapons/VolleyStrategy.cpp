#include "Components/Weapons/VolleyStrategy.h"

#include "Components/WeaponBaseComponent.h"

#include "Subsystems/ProjectilesSubsystem.h"

#include "Player/PlayingPlayer.h"

#include "Combat/BulletBase.h"

void UVolleyStrategy::StartFiring(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon || bRecharging)
	{
		return;
	}

	bRecharging = true;
	TWeakObjectPtr<UVolleyStrategy> WeakThis(this);
	TWeakObjectPtr<UWeaponBaseComponent> WeakWeapon(Weapon);

	for (int i = 0; i < MaxShots; i++)
	{
		float Delay = i * FireRate;

		FTimerHandle ShotTimer;
		Weapon->GetWorld()->GetTimerManager().SetTimer(ShotTimer,
			[WeakThis, WeakWeapon]()
			{
				if (WeakThis.IsValid() && WeakWeapon.IsValid())
				{
					WeakThis->ExecuteFire(WeakWeapon.Get());
				}
			}, Delay, false);
	}

	Weapon->GetWorld()->GetTimerManager().SetTimer(RechargeTimer,
		[WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->FinishRecharge();
			}
			
		}, (MaxShots * FireRate) + RechargeTime, false);
}
void UVolleyStrategy::StopFiring(
	UWeaponBaseComponent* Weapon)
{
	
}

void UVolleyStrategy::ExecuteFire(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	UWorld* World = Weapon->GetWorld();

	if (!World)
	{
		return;
	}

	UProjectilesSubsystem* Pool =
		World->GetGameInstance()
		->GetSubsystem<UProjectilesSubsystem>();

	if (!Pool)
	{
		return;
	}

	APlayingPlayer* Player =
		Cast<APlayingPlayer>(Weapon->GetOwner());

	if (!Player)
	{
		return;
	}

	float FinalDamage =
		Weapon->BaseDamage *
		Player->GetTotalDamageMultiplier();

	FVector Location =
		Weapon->GetComponentLocation();

	FVector Direction =
		Player->GetAimDirection();

	ABulletBase* Bullet =
		Pool->RequestBullet(
			Location,
			Direction,
			Weapon->BaseProjectileSpeed,
			true,
			FinalDamage,
			Location,
			Weapon->GetOwner()
		);

	if (Bullet)
	{
		Bullet->Tags.Add("BalaJugador");
	}
}

void UVolleyStrategy::FinishRecharge()
{
	CurrentShots = 0;
	bRecharging = false;
}