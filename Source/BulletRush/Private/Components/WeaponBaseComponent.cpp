// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponBaseComponent.h"
#include "Player/PlayingPlayer.h"
#include "../../Public/Subsystems/ProjectilesSubsystem.h"

// Sets default values for this component's properties
UWeaponBaseComponent::UWeaponBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	FireRate = 0.1f;

	// ...
}


// Called when the game starts
void UWeaponBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponBaseComponent::StartFiring()
{
    // Disparamos la primera bala inmediatamente
    ExecuteFire();

    // E iniciamos el temporizador para que siga disparando automáticamente
    GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &UWeaponBaseComponent::ExecuteFire, FireRate, true);
}

void UWeaponBaseComponent::StopFiring()
{
    // Limpiamos el temporizador cuando el jugador suelta el clic
    GetWorld()->GetTimerManager().ClearTimer(FiringTimer);
}

void UWeaponBaseComponent::ExecuteFire()
{
    // Lógica por defecto (Un solo disparo recto)
    // Los hijos sobrescribirán esto, pero es bueno tener una base funcional.
	// ABulletBase* RequestBullet(FVector Loc, FVector Dir, float Spd, bool bIsPlayer, float Damage, FVector SpawnLocation, AActor* Owner);

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
	APlayingPlayer* Player = Cast<APlayingPlayer>(GetOwner());
	if (!Player) return;
	float DamageMultiplier = Player->GetTotalDamageMultiplier();
	//float FinalDamage = BaseDamage * DamageMultiplier;
	float FinalDamage = BaseDamage;
	FVector Location = GetComponentLocation();
	FVector Direction = GetComponentRotation().Vector();
	ABulletBase* Bullet = PoolSubsystem->RequestBullet(Location, Direction, 1000.0f, true, FinalDamage, Location, GetOwner());
	if (!Bullet)
	{
		UE_LOG(LogTemp, Warning, TEXT("No se pudo RequestBullet"));
		return;
	}
	Bullet->Tags.Add("BalaJugador");
	
}

void UWeaponBaseComponent::HandleSilenceEvent(bool bIsSilenceActive)
{
	// Actualizamos el estado del arma según lo que dicte el director musical
	bIsSilenced = bIsSilenceActive;

	if (bIsSilenced)
	{
		UE_LOG(LogTemp, Warning, TEXT("El arma ha sido BLOQUEADA por el silencio."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("El silencio terminó. Arma DESBLOQUEADA."));
	}
}