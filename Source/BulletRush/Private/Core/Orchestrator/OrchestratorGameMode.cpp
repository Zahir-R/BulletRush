// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Enemies/Orchestrator/Orchestrator.h"
#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Core/BulletRushGameInstance.h"
#include "Core/BulletRushHUD.h"
#include "Player/PlayingPlayer.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AOrchestratorGameMode::AOrchestratorGameMode()
{
	LevelFacade = nullptr;
	DefaultPawnClass = APlayingPlayer::StaticClass();
	HUDClass = ABulletRushHUD::StaticClass();
}

void AOrchestratorGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		LevelFacade = GetWorld()->SpawnActor<AOrchestratorFacade>(AOrchestratorFacade::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (LevelFacade)
		{
			UE_LOG(LogTemp, Warning, TEXT("OrchestratorGameMode: Fachada del Nivel 5 instanciada con éxito."));
		}
	}
	APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (Player && Player->HealthComp)
		Player->HealthComp->OnDeath.AddDynamic(this, &AOrchestratorGameMode::OnPlayerDeath);

	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (!GI) return;

	FName MapName = FName(*GetWorld()->GetName());
	int32 VidasRestantes = GI->GetVidasRestantes(MapName);
}

void AOrchestratorGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Player && Player->HealthComp)
		Player->HealthComp->OnDeath.RemoveDynamic(this, &AOrchestratorGameMode::OnPlayerDeath);

	Super::EndPlay(EndPlayReason);
}

void AOrchestratorGameMode::OnPlayerDeath()
{
	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (!GI) return;

	FName MapName = FName(*GetWorld()->GetName());
	int32 VidasRestantes = GI->DecrementarVida(MapName);

	UE_LOG(LogTemp, Warning, TEXT("[OrchestratorGameMode] Jugador murio. Vidas restantes: %d"),
		VidasRestantes);

	if (VidasRestantes > 0)
	{
		UGameplayStatics::OpenLevel(this, MapName);
	}
	else
	{
		GI->OrchestratorLState = ELevelState::Normal;
		GI->ResetVidas(MapName);
		UGameplayStatics::OpenLevel(this, FName("Map_CupHeadMap"));
	}
}