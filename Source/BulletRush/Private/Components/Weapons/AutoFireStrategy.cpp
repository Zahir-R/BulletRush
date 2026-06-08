#include "Components/Weapons/AutoFireStrategy.h"
#include "Components/WeaponBaseComponent.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Player/PlayingPlayer.h"
#include "Combat/BulletBase.h"

void UAutoFireStrategy::StartFiring(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	ExecuteFire(Weapon);

	TWeakObjectPtr<UAutoFireStrategy> WeakThis(this);
	TWeakObjectPtr<UWeaponBaseComponent> WeakWeapon(Weapon);

	Weapon->GetWorld()->GetTimerManager().SetTimer(Weapon->FiringTimer,
		[WeakThis, WeakWeapon]()
		{
			if (WeakThis.IsValid() && WeakWeapon.IsValid())
			{
				WeakThis->ExecuteFire(WeakWeapon.Get());
			}
		}, WeakWeapon->FireRate, true);
}

void UAutoFireStrategy::StopFiring(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	if (Weapon->FiringTimer.IsValid())
	{
		Weapon->GetWorld()->GetTimerManager().ClearTimer(Weapon->FiringTimer);
	}
}

void UAutoFireStrategy::ExecuteFire(
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
		Weapon->GetComponentRotation().Vector();

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