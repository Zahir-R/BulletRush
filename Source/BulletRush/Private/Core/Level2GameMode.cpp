#include "Core/Level2GameMode.h"
#include "Core/BulletRushGameModeBase.h"
#include "VaultKeeper/core/Level21Facade.h"
#include "VaultKeeper/core/Level2SFacade.h"
#include "VaultKeeper/core/VaultKeeperFacade.h"
#include "Engine/World.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Core/BulletRushGameInstance.h"

ALevel2GameMode::ALevel2GameMode()
{
    DefaultPawnClass = APlayingPlayer::StaticClass();
}

void ALevel2GameMode::BeginPlay()
{
    Super::BeginPlay();
    DetectAndActivateFacade();
    if (GetWorld())
    {
        ABulletRushGameModeBase::SpawnPowerUpsForLevel(GetWorld(), FName(GetWorld()->GetMapName()));
    }
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