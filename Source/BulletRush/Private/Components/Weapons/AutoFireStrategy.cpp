#include "Components/Weapons/AutoFireStrategy.h"
#include "Components/WeaponBaseComponent.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/BulletBase.h"

UAutoFireStrategy::UAutoFireStrategy()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("SoundWave'/Game/BlasterSFX/New_Project.New_Project'")
	);

	if (SoundAsset.Succeeded())
	{
		FireSound = SoundAsset.Object;
	}
}

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

		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				Weapon,
				FireSound,
				Location
			);
		}
	}
}