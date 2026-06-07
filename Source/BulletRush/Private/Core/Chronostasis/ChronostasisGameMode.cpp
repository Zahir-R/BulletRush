#include "Core/Chronostasis/ChronostasisGameMode.h"
#include "Core/Chronostasis/ChronostasisNormalFacade.h"
#include "Core/Chronostasis/ChronostasisSecretFacade.h"
#include "Core/Chronostasis/ChronostasisBossFacade.h"
#include "Core/BulletRushGameModeBase.h"
#include "Core/BulletRushGameInstance.h"
#include "Core/BulletRushHUD.h"
#include "Map/MapChronoTesting.h"
#include "Player/PlayingPlayer.h"
#include "Components/HealthComponent.h"
#include "Subsystems/MusicManagerSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AChronostasisGameMode::AChronostasisGameMode()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
	HUDClass = ABulletRushHUD::StaticClass();
}

void AChronostasisGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		GetWorld()->SpawnActor<AMapChronoTesting>(
			AMapChronoTesting::StaticClass(),
			FVector::ZeroVector, FRotator::ZeroRotator);

		ABulletRushGameModeBase::SpawnPowerUpsForLevel(
			GetWorld(), FName(GetWorld()->GetMapName()));
	}

	APlayingPlayer* Player = Cast<APlayingPlayer>(
		UGameplayStatics::GetPlayerPawn(this, 0));

	if (Player && Player->HealthComp)
		Player->HealthComp->OnDeath.AddDynamic(this, &AChronostasisGameMode::OnPlayerDeath);

	DetectAndActivateFacade();
}

void AChronostasisGameMode::DetectAndActivateFacade()
{
	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChronostasisGameMode] GameInstance no encontrado"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	switch (GI->ChronostasisState)
	{
	case ELevelState::Normal:
		FacadeNormal = World->SpawnActor<AChronostasisNormalFacade>(
			AChronostasisNormalFacade::StaticClass(),
			FVector::ZeroVector, FRotator::ZeroRotator);
		if (FacadeNormal) FacadeNormal->StartLevel();
		UE_LOG(LogTemp, Warning, TEXT("[ChronostasisGameMode] Modo: Normal"));
		break;

	case ELevelState::Secret:
		FacadeSecret = World->SpawnActor<AChronostasisSecretFacade>(
			AChronostasisSecretFacade::StaticClass(),
			FVector::ZeroVector, FRotator::ZeroRotator);
		if (FacadeSecret) FacadeSecret->StartLevel();
		UE_LOG(LogTemp, Warning, TEXT("[ChronostasisGameMode] Modo: Secret"));
		break;

	case ELevelState::Boss:
		FacadeBoss = World->SpawnActor<AChronostasisBossFacade>(
			AChronostasisBossFacade::StaticClass(),
			FVector::ZeroVector, FRotator::ZeroRotator);
		if (FacadeBoss) FacadeBoss->StartLevel();
		UE_LOG(LogTemp, Warning, TEXT("[ChronostasisGameMode] Modo: Boss"));
		break;
	}
}

void AChronostasisGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Player && Player->HealthComp)
		Player->HealthComp->OnDeath.RemoveDynamic(this, &AChronostasisGameMode::OnPlayerDeath);

	Super::EndPlay(EndPlayReason);
}

void AChronostasisGameMode::OnPlayerDeath()
{
	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
	{
		Music->SavePlaybackPosition();
		Music->NotifyLevelTravel();
	}

	if (!GI) return;

	FName MapName = FName(*GetWorld()->GetName());
	int32 VidasRestantes = GI->DecrementarVida(MapName);

	UE_LOG(LogTemp, Warning, TEXT("[ChronostasisGameMode] Jugador murio. Vidas restantes: %d"),
		VidasRestantes);

	if (VidasRestantes > 0)
	{
		UGameplayStatics::OpenLevel(this, MapName);
	}
	else
	{
		GI->ChronostasisState = ELevelState::Normal;
		UGameplayStatics::OpenLevel(this, FName("Map_CupHeadMap"));
	}
}
