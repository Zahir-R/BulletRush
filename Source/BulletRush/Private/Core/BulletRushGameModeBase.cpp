#include "Core/BulletRushGameModeBase.h"
#include "Player/PlayingPlayer.h"
#include "Player/TopDownPlayer.h"
#include "Engine/World.h"
#include "Core/Chronostasis/ChronostasisEnemyFactory.h"
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
		
	}
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