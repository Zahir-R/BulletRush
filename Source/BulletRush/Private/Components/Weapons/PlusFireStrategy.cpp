#include "Components/Weapons/PlusFireStrategy.h"
#include "Components/WeaponBaseComponent.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/BulletBase.h"

UPlusFireStrategy::UPlusFireStrategy()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
		TEXT("SoundWave'/Game/BlasterSFX/New_Project.New_Project'")
	);

	if (SoundAsset.Succeeded())
	{
		FireSound = SoundAsset.Object;
	}
}

void UPlusFireStrategy::StartFiring(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	ExecuteFire(Weapon);
	TWeakObjectPtr<UPlusFireStrategy> WeakThis(this);
	TWeakObjectPtr<UWeaponBaseComponent> WeakWeapon(Weapon);

	Weapon->GetWorld()->GetTimerManager().SetTimer(Weapon->FiringTimer,
		[WeakThis, WeakWeapon]()
		{
			if (WeakThis.IsValid() && WeakWeapon.IsValid())
			{
				WeakThis->ExecuteFire(WeakWeapon.Get());
			}
		}, NewFireRate, true);
}

void UPlusFireStrategy::StopFiring(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon)
	{
		return;
	}
	Weapon->GetWorld()->GetTimerManager().ClearTimer(Weapon->FiringTimer);
	
}

void UPlusFireStrategy::ExecuteFire(
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
		Player->GetTotalDamageMultiplier() + AdditionalDamage;

	FVector Location =
		Weapon->GetComponentLocation();

	FRotator BaseRotation =
		Player->GetAimDirection().Rotation();

	TArray<FRotator> Rotations;

	Rotations.Add(BaseRotation);

	Rotations.Add(
		BaseRotation + FRotator(0.f, SpreadAngle, 0.f));

	Rotations.Add(
		BaseRotation + FRotator(0.f, -SpreadAngle, 0.f));

	Rotations.Add(
		BaseRotation + FRotator(SpreadAngle, 0.f, 0.f));

	Rotations.Add(
		BaseRotation + FRotator(-SpreadAngle, 0.f, 0.f));

	for (FRotator Rot : Rotations)
	{
		FVector Direction = Rot.Vector();

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
}