#include "Core/Chronostasis/ChronostasisBossManager.h"
#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Core/Chronostasis/ChargerFactory.h"
#include "Core/Chronostasis/LinkerFactory.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"
#include "Enemies/EnemyBase.h"
#include "Core/BulletRushHUD.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UChronostasisBossManager::Initialize(AChronostasisFacade* Owner, TSubclassOf<ASerXBoss> BossClass, const FVector& SpawnLocation)
{
    OwnerFacade = Owner;
    SerXBossClass = BossClass ? BossClass : ASerXBoss::StaticClass();
    BossArenaSpawnLocation = SpawnLocation;

    BossChargerFactory = NewObject<UChargerFactory>(this);
    BossLinkerFactory = NewObject<ULinkerFactory>(this);
}

void UChronostasisBossManager::StartBossFight()
{
    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (!World || !SerXBossClass) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ASerXBoss* Boss = World->SpawnActor<ASerXBoss>(SerXBossClass, BossArenaSpawnLocation, FRotator::ZeroRotator, Params);
    if (Boss)
    {
        Boss->OnEnemyDeath.AddDynamic(this, &UChronostasisBossManager::OnBossKilled);
        Boss->SetChargerFactory(BossChargerFactory);
        Boss->SetLinkerFactory(BossLinkerFactory);

        APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
        ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
        if (HUD)
        {
            HUD->ShowMessage(TEXT("BOSS FIGHT"), 3.f);
        }
    }
}

void UChronostasisBossManager::OnBossKilled(AEnemyBase* Boss)
{
    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (World)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
        ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
        if (HUD)
        {
            HUD->ShowMessage(TEXT("VICTORY"), 5.f);
        }

        UProjectilesSubsystem* ProjSys = World->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
        if (ProjSys) ProjSys->ReturnAllActiveBullets();
    }

    OnBossDefeated.Broadcast();
}
