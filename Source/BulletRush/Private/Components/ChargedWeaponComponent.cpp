// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ChargedWeaponComponent.h"
#include "Player/PlayingPlayer.h"
#include "../../Public/Subsystems/ProjectilesSubsystem.h"

void UChargedWeaponComponent::StartFiring()
{
	Damage = DamageBase;
    GetWorld()->GetTimerManager().SetTimer(FiringTimer, [this]()
        {
            // Incrementamos el daño mientras se mantenga el clic
			if (Damage < MaxDamage)
			{
				Damage *= 2.0f;
				UE_LOG(LogTemp, Warning, TEXT("Daño aumentado, ahora el daño es: %f"), Damage);
			}
        }, 1.0f, true);
}
void UChargedWeaponComponent::StopFiring()
{
    // Limpiamos el temporizador cuando el jugador suelta el clic
	//ExecuteFire(); // Disparamos la bala cargada al soltar el clic
    GetWorld()->GetTimerManager().ClearTimer(FiringTimer);
}

/*void UChargedWeaponComponent::ExecuteFire()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("No World"));
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No GameInstance"));
		return;
	}

	UProjectilesSubsystem* PoolSubsystem = World->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();

	if (!PoolSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("No se pudo generar la bala o no hay disponibles en el pool"));
		return;
	}

	// Player only ?
	// Also, duplicate with WeaponBaseComponent www
	APlayingPlayer* Player = Cast<APlayingPlayer>(GetOwner());
	if (!Player) return;

	float DamageMultiplier = Player->GetTotalDamageMultiplier();
	float FinalDamage = Damage * DamageMultiplier;

	FVector Location = GetComponentLocation();
	FVector Direction = GetComponentRotation().Vector();
	ABulletBase* Bullet = PoolSubsystem->RequestBullet(Location, Direction, 1000.0f, true, FinalDamage, Location, GetOwner());
	if (!Bullet)
	{
		UE_LOG(LogTemp, Warning, TEXT("No se pudo RequestBullet"));
		return;
	}
	Bullet->Tags.Add("BalaJugador");
	Damage = DamageBase; // Reset
}*/