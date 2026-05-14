#include "../../Public/Core/BulletRushGameModeBase.h"
#include "../../Public/Player/PlayingPlayer.h"
#include "../../Public/Player/TopDownPlayer.h"
#include "Engine/World.h"

#include "../../Public/Map/PortalManager.h"
#include <Kismet/GameplayStatics.h>

#include "../../Public/Map/LevelPortal.h"

ABulletRushGameModeBase::ABulletRushGameModeBase()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
	
}

void ABulletRushGameModeBase::BeginPlay() 
{
	Super::BeginPlay();
	//para testeo 
	if (GetWorld()) {
		ALevelPortal* NivelRetorno = GetWorld()->SpawnActor<ALevelPortal>(ALevelPortal::StaticClass(), FVector(300.0f, 300.0f, 50.0f), FRotator::ZeroRotator);
		if (NivelRetorno)
		{
			NivelRetorno->TargetLevelName = "Map_CupHeadMap";
		}
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
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