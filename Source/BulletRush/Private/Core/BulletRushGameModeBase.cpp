#include "Core/BulletRushGameModeBase.h"
#include "Core/BulletRushHUD.h"  
#include "Buffs/PowerUpManager.h"
#include "Buffs/DamagePowerUp.h"
#include "Buffs/HealthPowerUp.h"
#include "Buffs/SpeedPowerUp.h"
#include "Buffs/UltimatePowerUp.h"
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
	HUDClass = ABulletRushHUD::StaticClass();
}

void ABulletRushGameModeBase::BeginPlay() 
{
	Super::BeginPlay();
    if (GetWorld()) {
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
	Classes.Add(AUltimatePowerUp::StaticClass());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APowerUpManager* Manager = World->SpawnActor<APowerUpManager>(APowerUpManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (Manager) Manager->Initialize(Classes, FVector(5000.0f, 5000.0f, 2000.0f));
	return Manager;
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
