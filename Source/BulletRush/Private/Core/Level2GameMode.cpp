#include "Core/Level2GameMode.h"
#include "Core/BulletRushGameModeBase.h"
#include "Core/BulletRushHUD.h"
#include "VaultKeeper/core/Level21Facade.h"
#include "VaultKeeper/core/Level2SFacade.h"
#include "VaultKeeper/core/VaultKeeperFacade.h"
#include "Engine/World.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Core/BulletRushGameInstance.h"
#include "Subsystems/MusicManagerSubsystem.h"

ALevel2GameMode::ALevel2GameMode()
{
    DefaultPawnClass = APlayingPlayer::StaticClass();
    HUDClass = ABulletRushHUD::StaticClass();
}

void ALevel2GameMode::BeginPlay()
{
    Super::BeginPlay();
    
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    if (Player && Player->HealthComp)
        Player->HealthComp->OnDeath.AddDynamic(this, &ALevel2GameMode::OnPlayerDeath);

    DetectAndActivateFacade();

    if (GetWorld())
        ABulletRushGameModeBase::SpawnPowerUpsForLevel(
            GetWorld(), FName(GetWorld()->GetMapName()));
}

void ALevel2GameMode::DetectAndActivateFacade()
{
    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("[Level2GameMode] GameInstance no encontrado"));
        return;
    }

    switch (GI->Level2State)
    {
    case ELevelState::Normal:
        Facade21 = GetWorld()->SpawnActor<ALevel21Facade>(
            ALevel21Facade::StaticClass(),
            FVector::ZeroVector, FRotator::ZeroRotator);
        if (Facade21) Facade21->StartLevel();
        UE_LOG(LogTemp, Warning, TEXT("[Level2GameMode] Modo: 2-1 Normal"));
        break;

    case ELevelState::Secret:
        Facade2S = GetWorld()->SpawnActor<ALevel2SFacade>(
            ALevel2SFacade::StaticClass(),
            FVector::ZeroVector, FRotator::ZeroRotator);
        if (Facade2S) Facade2S->StartLevel();
        UE_LOG(LogTemp, Warning, TEXT("[Level2GameMode] Modo: 2-S Secreto"));
        break;

    case ELevelState::Boss:
        FacadeVK = GetWorld()->SpawnActor<AVaultKeeperFacade>(
            AVaultKeeperFacade::StaticClass(),
            FVector::ZeroVector, FRotator::ZeroRotator);
        if (FacadeVK) FacadeVK->StartLevel();
        UE_LOG(LogTemp, Warning, TEXT("[Level2GameMode] Modo: 2-2 Boss"));
        break;
    }
}
void ALevel2GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (Player && Player->HealthComp)
        Player->HealthComp->OnDeath.RemoveDynamic(this, &ALevel2GameMode::OnPlayerDeath);

    Super::EndPlay(EndPlayReason);
}

void ALevel2GameMode::OnPlayerDeath()
{
    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
        Music->NotifyLevelTravel();

    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (!GI) return;

    FName MapName = FName(*GetWorld()->GetName());
    int32 VidasRestantes = GI->DecrementarVida(MapName);

    UE_LOG(LogTemp, Warning, TEXT("[Level2GameMode] Jugador murio. Vidas restantes: %d"),
        VidasRestantes);

    if (VidasRestantes > 0)
    {
        // Recarga el mismo nivel
        UGameplayStatics::OpenLevel(this, MapName);
    }
    else
    {
        // Sin vidas ? resetea estado y vuelve al mapa
        GI->Level2State = ELevelState::Normal;
        UGameplayStatics::OpenLevel(this, FName("Map_CupHeadMap"));
    }
}