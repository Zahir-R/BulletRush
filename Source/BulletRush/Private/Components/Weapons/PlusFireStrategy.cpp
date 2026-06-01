#include "Components/Weapons/PlusFireStrategy.h"
#include "Components/WeaponBaseComponent.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Player/PlayingPlayer.h"
#include "Combat/BulletBase.h"

void UPlusFireStrategy::StartFiring(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	ExecuteFire(Weapon);

	Weapon->GetWorld()->GetTimerManager().SetTimer(
		Weapon->FiringTimer,
		FTimerDelegate::CreateLambda([this, Weapon]()
			{
				ExecuteFire(Weapon);
			}),
		NewFireRate,
		true
	);
}

void UPlusFireStrategy::StopFiring(
	UWeaponBaseComponent* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	Weapon->GetWorld()->GetTimerManager().ClearTimer(
		Weapon->FiringTimer
	);
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
		Weapon->GetComponentRotation();

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
		}
	}
}