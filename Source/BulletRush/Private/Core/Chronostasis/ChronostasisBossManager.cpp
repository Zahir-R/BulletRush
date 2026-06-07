#include "Core/Chronostasis/ChronostasisBossManager.h"
#include "Core/Chronostasis/GenericEnemyFactory.h"
#include "Enemies/Chronostasis/ChronostasisCharger.h"
#include "Enemies/Chronostasis/ChronostasisLinker.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Enemies/EnemyBase.h"
#include "Core/BulletRushHUD.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UChronostasisBossManager::Initialize(AActor* Owner, TSubclassOf<ASerXBoss> BossClass, const FVector& SpawnLocation)
{
    OwnerActor = Owner;
    SerXBossClass = BossClass ? BossClass : ASerXBoss::StaticClass();
    BossArenaSpawnLocation = SpawnLocation;

    BossLinkerFactory = CreateGenericFactory<AChronostasisLinker>(this);

    MinionFactories.Add(CreateGenericFactory<AChronostasisDrone>(this));
    MinionFactories.Add(CreateGenericFactory<AChronostasisExpansive>(this));
    MinionFactories.Add(CreateGenericFactory<AChronostasisMass>(this));
    MinionFactories.Add(CreateGenericFactory<AChronostasisCharger>(this));
}

void UChronostasisBossManager::StartBossFight()
{
    UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
    if (!World || !SerXBossClass) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ASerXBoss* Boss = World->SpawnActor<ASerXBoss>(SerXBossClass, BossArenaSpawnLocation, FRotator::ZeroRotator, Params);
    if (Boss)
    {
        Boss->OnEnemyDeath.AddDynamic(this, &UChronostasisBossManager::OnBossKilled);
        Boss->SetLinkerFactory(BossLinkerFactory);
        for (auto* Factory : MinionFactories)
        {
            Boss->AddMinionFactory(Factory);
        }

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
    UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
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
