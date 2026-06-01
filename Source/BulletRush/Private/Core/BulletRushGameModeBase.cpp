#include "Core/BulletRushGameModeBase.h"
#include "Buffs/PowerUpManager.h"
#include "Buffs/DamagePowerUp.h"
#include "Buffs/HealthPowerUp.h"
#include "Buffs/SpeedPowerUp.h"
#include "Player/PlayingPlayer.h"
#include "Player/TopDownPlayer.h"
#include "Engine/World.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "Enemies/EnemyBase.h"
#include "Map/LevelPortal.h"
#include "Map/PortalManager.h"
#include <Kismet/GameplayStatics.h>


ABulletRushGameModeBase::ABulletRushGameModeBase()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
	
}

void ABulletRushGameModeBase::BeginPlay() 
{
	Super::BeginPlay();
	//para testeo 
    if (GetWorld()) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform SpawnTransform(FRotator::ZeroRotator, FVector(300.0f, 300.0f, 50.0f));
		ALevelPortal* NivelRetorno = GetWorld()->SpawnActor<ALevelPortal>(ALevelPortal::StaticClass(), SpawnTransform, SpawnParams);
		if (NivelRetorno)
		{
			NivelRetorno->TargetLevelName = "Map_CupHeadMap";
		}
		//spawneamos el nivel para volver al cuphead

		SpawnPowerUpsForLevel(GetWorld(), FName(GetWorld()->GetMapName()));
	}
}

APowerUpManager* ABulletRushGameModeBase::SpawnPowerUpsForLevel(UWorld* World, FName LevelName)
{
	if (!World) return nullptr;

	// Exclude hub level
	if (LevelName.ToString().Contains("CupHead"))
		return nullptr;

	bool bSpawn = false;
	TArray<TSubclassOf<APowerUpBase>> Classes;

	if (LevelName.ToString().Contains("Map_01Boss") ||
		LevelName.ToString().Contains("Map_02Boss") ||
		LevelName.ToString().Contains("Map_03Boss") ||
		LevelName.ToString().Contains("Map_04Boss") ||
		LevelName.ToString().Contains("Map_05Boss"))
	{
		bSpawn = true;
	}

	if (!bSpawn) return nullptr;

	Classes.Add(AHealthPowerUp::StaticClass());
	Classes.Add(ADamagePowerUp::StaticClass());
	Classes.Add(ASpeedPowerUp::StaticClass());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APowerUpManager* Manager = World->SpawnActor<APowerUpManager>(APowerUpManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (Manager) Manager->Initialize(Classes, FVector(1000.0f, 1000.0f, 400.0f));
	return Manager;
}

UFUNCTION(Exec)
void ABulletRushGameModeBase::DealDamageToTarget(float Damage)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("DealDamageToTarget: No se encontr� PlayerController"));
		return;
	}

	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (!Hit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("DealDamageToTarget: No se impact� ning�n objeto. Apunta a un actor visible con el cursor."));
		return;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DealDamageToTarget: El impacto no tiene actor asociado"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("DealDamageToTarget: Aplicando %.1f de da�o a [%s]"), Damage, *HitActor->GetName());

	UGameplayStatics::ApplyDamage(HitActor, Damage, PC, this, UDamageType::StaticClass());
}
//para testeo noma aqui
/*
UClass* ABulletRushGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{

	if (GetWorld())
	{
		FString LevelName = GetWorld()->GetMapName();
		if (LevelName.Contains("Map_CupHeadMap"))
		{
			// Si estamos en tu mapa Cuphead
			return ATopDownPlayer::StaticClass();
		}
	}
	// Si estamos en otro nivel  usa  playingplayer por el momento
	return APlayingPlayer::StaticClass();

}
*/
