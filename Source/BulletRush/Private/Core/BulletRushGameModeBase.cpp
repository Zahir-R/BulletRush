#include "../../Public/Core/BulletRushGameModeBase.h"
#include "../../Public/Player/PlayingPlayer.h"
#include "../../Public/Player/TopDownPlayer.h"
#include "Engine/World.h"

#include "../../Public/Map/PortalManager.h"
#include <Kismet/GameplayStatics.h>

ABulletRushGameModeBase::ABulletRushGameModeBase()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
	
}
//para testeo noma aqui
void ABulletRushGameModeBase::BeginPlay() 
{
	Super::BeginPlay();
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
	
	
}
//para testeo noma aqui
UClass* ABulletRushGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	
	if (GetWorld())
	{
		FString LevelName = GetWorld()->GetMapName();
		// NOTA: Usamos "Contains" porque al darle Play en el editor, Unreal a veces 
		// le pone nombres que no coinciden con el nombre del mapa
		if (LevelName.Contains("Map_CupHeadMap"))
		{
			// Si estamos en tu mapa, usamos nave TopDown
			return ATopDownPlayer::StaticClass();
		}
	}
	// Si estamos en otro nivel  usa  playingplayer por el momento 
	return APlayingPlayer::StaticClass();
	
}

UFUNCTION(Exec)
void ABulletRushGameModeBase::DealDamageToTarget(float Damage)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("DealDamageToTarget: No se encontró PlayerController"));
		return;
	}

	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (!Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("DealDamageToTarget: No se impactó ningún objeto. Apunta a un actor visible con el cursor."));
		return;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DealDamageToTarget: El impacto no tiene actor asociado"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("DealDamageToTarget: Aplicando %.1f de daño a [%s]"), Damage, *HitActor->GetName());

	UGameplayStatics::ApplyDamage(HitActor, Damage, PC, this, UDamageType::StaticClass());
}