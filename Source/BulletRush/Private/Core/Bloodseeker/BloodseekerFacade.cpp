#include "Core/Bloodseeker/BloodseekerFacade.h"
#include "Enemies/Bloodseeker/KamikazeEnemy.h"
#include "Enemies/Bloodseeker/LineWelderEnemy.h"
#include "Enemies/Bloodseeker/GravitySiphonEnemy.h"
#include "Enemies/EnemyBase.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ABloodseekerFacade::ABloodseekerFacade()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentWaveIndex = 0;
    RemainingEnemiesInWave = 0;
    bIsBossPhase = false;
}

void ABloodseekerFacade::BeginPlay()
{
    Super::BeginPlay();
}

void ABloodseekerFacade::SetupDefaultWaves()
{
    HordasWaves.Empty();

    FBloodseekerWaveConfig Wave1;
    Wave1.KamikazeCount = 3;
    Wave1.LineWelderCount = 0;
    Wave1.GravitySiphonCount = 0;
    Wave1.DelayBetweenSpawns = 0.5f;
    HordasWaves.Add(Wave1);

    FBloodseekerWaveConfig Wave2;
    Wave2.KamikazeCount = 5;
    Wave2.LineWelderCount = 2;
    Wave2.GravitySiphonCount = 0;
    Wave2.DelayBetweenSpawns = 0.4f;
    HordasWaves.Add(Wave2);

    FBloodseekerWaveConfig Wave3;
    Wave3.KamikazeCount = 8;
    Wave3.LineWelderCount = 3;
    Wave3.GravitySiphonCount = 1;
    Wave3.DelayBetweenSpawns = 0.3f;
    HordasWaves.Add(Wave3);

    FBloodseekerWaveConfig Wave4;
    Wave4.KamikazeCount = 10;
    Wave4.LineWelderCount = 4;
    Wave4.GravitySiphonCount = 2;
    Wave4.DelayBetweenSpawns = 0.25f;
    HordasWaves.Add(Wave4);
}

void ABloodseekerFacade::SetupDefaultBossWaves()
{
    BossWaves.Empty();

    FBloodseekerWaveConfig BossWave1;
    BossWave1.KamikazeCount = 2;
    BossWave1.LineWelderCount = 1;
    BossWave1.DelayBetweenSpawns = 0.3f;
    BossWaves.Add(BossWave1);

    FBloodseekerWaveConfig BossWave2;
    BossWave2.KamikazeCount = 3;
    BossWave2.LineWelderCount = 2;
    BossWave2.DelayBetweenSpawns = 0.4f;
    BossWaves.Add(BossWave2);
}

void ABloodseekerFacade::StartGame()
{
    if (!EnemyFactory)
    {
        EnemyFactory = NewObject<UBloodseekerEnemyFactory>(this);
    }

    SetupDefaultWaves();
    SetupDefaultBossWaves();
    CurrentWaveIndex = 0;
    bIsBossPhase = false;
    UE_LOG(LogTemp, Warning, TEXT("[Facade] StartGame - Oleadas configuradas: %d. Iniciando oleada 0..."), HordasWaves.Num());
    StartWave(CurrentWaveIndex);
}

void ABloodseekerFacade::StartWave(int32 Index)
{
    if (!GetWorld() || Index >= HordasWaves.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Facade] StartWave(%d) - Todas las oleadas completadas!"), Index);
        OnAllWavesComplete.Broadcast();
        return;
    }

    const FBloodseekerWaveConfig& Config = HordasWaves[Index];
    RemainingEnemiesInWave = Config.KamikazeCount + Config.LineWelderCount + Config.GravitySiphonCount;
    CurrentWaveIndex = Index;
    UE_LOG(LogTemp, Warning, TEXT("[Facade] StartWave(%d) - K:%d L:%d G:%d Total:%d"), Index, Config.KamikazeCount, Config.LineWelderCount, Config.GravitySiphonCount, RemainingEnemiesInWave);

    SpawnEnemiesForConfig(Config);
}

void ABloodseekerFacade::SpawnEnemiesForConfig(const FBloodseekerWaveConfig& Config)
{
    SpawnTimerHandles.Empty();

    UE_LOG(LogTemp, Warning, TEXT("[Facade] SpawnEnemiesForConfig - Creando %d Kamikaze, %d LineWelder, %d GravitySiphon"),
        Config.KamikazeCount, Config.LineWelderCount, Config.GravitySiphonCount);

    for (int32 i = 0; i < Config.KamikazeCount; i++)
    {
        FTimerHandle NewHandle;
        float Delay = Config.DelayBetweenSpawns * i;
        if (Delay <= 0.0f) Delay = 0.01f;
        UE_LOG(LogTemp, Warning, TEXT("[Facade] SpawnEnemiesForConfig - Timer Kamikaze[%d] delay=%.1f"), i, Delay);
        TWeakObjectPtr<ABloodseekerFacade> WeakThis(this);
        GetWorld()->GetTimerManager().SetTimer(
            NewHandle,
            [WeakThis, i]()
            {
                ABloodseekerFacade* Self = WeakThis.Get();
                UE_LOG(LogTemp, Warning, TEXT("[Facade] TimerCallback Kamikaze[%d] ejecutado - IsValid:%d World:%s"),
                    i, IsValid(Self) ? 1 : 0, Self && Self->GetWorld() ? TEXT("OK") : TEXT("NULL"));
                if (!Self || !Self->GetWorld()) return;
                Self->SpawnSingleKamikaze(Self->GetRandomSpawnLocation());
            },
            Delay,
            false
        );
        SpawnTimerHandles.Add(NewHandle);
    }

    float KamikazeEnd = Config.KamikazeCount * Config.DelayBetweenSpawns;
    for (int32 i = 0; i < Config.LineWelderCount; i++)
    {
        FTimerHandle NewHandle;
        float Delay = Config.DelayBetweenSpawns * i + KamikazeEnd;
        if (Delay <= 0.0f) Delay = 0.01f;
        UE_LOG(LogTemp, Verbose, TEXT("[Facade] Timer LineWelder[%d] delay=%.1f"), i, Delay);
        TWeakObjectPtr<ABloodseekerFacade> WeakThis_LW(this);
        GetWorld()->GetTimerManager().SetTimer(
            NewHandle,
            [WeakThis_LW, i]()
            {
                ABloodseekerFacade* Self = WeakThis_LW.Get();
                UE_LOG(LogTemp, Verbose, TEXT("[Facade] TimerCallback LineWelder[%d] ejecutado"), i);
                if (!Self || !Self->GetWorld()) return;
                Self->SpawnSingleLineWelder(Self->GetRandomSpawnLocation());
            },
            Delay,
            false
        );
        SpawnTimerHandles.Add(NewHandle);
    }

    float LineWelderEnd = Config.LineWelderCount * Config.DelayBetweenSpawns + KamikazeEnd;
    for (int32 i = 0; i < Config.GravitySiphonCount; i++)
    {
        FTimerHandle NewHandle;
        float Delay = Config.DelayBetweenSpawns * i + LineWelderEnd;
        if (Delay <= 0.0f) Delay = 0.01f;
        UE_LOG(LogTemp, Verbose, TEXT("[Facade] Timer GravitySiphon[%d] delay=%.1f"), i, Delay);
        TWeakObjectPtr<ABloodseekerFacade> WeakThis_GS(this);
        GetWorld()->GetTimerManager().SetTimer(
            NewHandle,
            [WeakThis_GS, i]()
            {
                ABloodseekerFacade* Self = WeakThis_GS.Get();
                UE_LOG(LogTemp, Verbose, TEXT("[Facade] TimerCallback GravitySiphon[%d] ejecutado"), i);
                if (!Self || !Self->GetWorld()) return;
                Self->SpawnSingleGravitySiphon(Self->GetRandomSpawnLocation());
            },
            Delay,
            false
        );
        SpawnTimerHandles.Add(NewHandle);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Facade] SpawnEnemiesForConfig - %d timers creados"), SpawnTimerHandles.Num());
}

void ABloodseekerFacade::SpawnSingleKamikaze(const FVector& Location)
{
    UE_LOG(LogTemp, Warning, TEXT("[Facade] >>> SpawnSingleKamikaze INICIO - Location:%s World:%s Factory:%s"),
        *Location.ToString(), GetWorld() ? TEXT("OK") : TEXT("NULL"), EnemyFactory ? TEXT("OK") : TEXT("NULL"));

    if (!GetWorld() || !EnemyFactory)
    {
        UE_LOG(LogTemp, Error, TEXT("[Facade] SpawnSingleKamikaze FALLA - World:%s Factory:%s"), GetWorld() ? TEXT("OK") : TEXT("NULL"), EnemyFactory ? TEXT("OK") : TEXT("NULL"));
        return;
    }

    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    FVector ActualSpawnLocation = Location;
    if (Player)
    {
        FVector PlayerLoc = Player->GetActorLocation();
        FVector PlayerForward = Player->GetActorForwardVector();
        PlayerForward.Z = 0.0f;
        if (PlayerForward.IsNearlyZero()) PlayerForward = FVector::ForwardVector;
        PlayerForward.Normalize();

        float HorzAngle = FMath::FRandRange(-120.0f, 120.0f);
        FVector HorzDir = PlayerForward.RotateAngleAxis(HorzAngle, FVector::UpVector);
        float Distance = FMath::FRandRange(1800.0f, 2500.0f);
        float HeightOffset = FMath::FRandRange(-300.0f, 300.0f);

        ActualSpawnLocation = PlayerLoc + HorzDir * Distance;
        ActualSpawnLocation.Z += HeightOffset;
    }

    AKamikazeEnemy* Enemy = EnemyFactory->CreateKamikaze(GetWorld(), ActualSpawnLocation);
    if (Enemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Facade] Kamikaze spawneado en %s - Actor:%s"), *ActualSpawnLocation.ToString(), *Enemy->GetName());
        if (Player)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Facade] Inicializando kamikaze hacia jugador"));
            Enemy->InitializeArc(ActualSpawnLocation, Player->GetActorLocation());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Facade] NO HAY PLAYER para inicializar arco!"));
        }
        Enemy->OnEnemyDeath.AddDynamic(this, &ABloodseekerFacade::OnEnemyKilled);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[Facade] ERROR: No se pudo spawnear Kamikaze en %s"), *Location.ToString());
    }

    UE_LOG(LogTemp, Warning, TEXT("[Facade] <<< SpawnSingleKamikaze FIN"));
}

void ABloodseekerFacade::SpawnSingleLineWelder(const FVector& Location)
{
    if (!GetWorld() || !EnemyFactory) return;

    ALineWelderEnemy* Enemy = EnemyFactory->CreateLineWelder(GetWorld(), Location);
    if (Enemy)
    {
        Enemy->OnEnemyDeath.AddDynamic(this, &ABloodseekerFacade::OnEnemyKilled);
    }
}

void ABloodseekerFacade::SpawnSingleGravitySiphon(const FVector& Location)
{
    if (!GetWorld() || !EnemyFactory) return;

    AGravitySiphonEnemy* Enemy = EnemyFactory->CreateGravitySiphon(GetWorld(), Location);
    if (Enemy)
    {
        Enemy->OnEnemyDeath.AddDynamic(this, &ABloodseekerFacade::OnEnemyKilled);
    }
}

FVector ABloodseekerFacade::GetRandomSpawnLocation()
{
    static const TArray<FVector> SpawnPoints = {
        FVector(800.0f, 0.0f, 100.0f),
        FVector(-800.0f, 0.0f, 100.0f),
        FVector(0.0f, 800.0f, 100.0f),
        FVector(0.0f, -800.0f, 100.0f),
        FVector(600.0f, 600.0f, 100.0f),
        FVector(-600.0f, 600.0f, 100.0f),
        FVector(600.0f, -600.0f, 100.0f),
        FVector(-600.0f, -600.0f, 100.0f)
    };

    int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
    return SpawnPoints[Index];
}

void ABloodseekerFacade::OnEnemyKilled(AEnemyBase* DeadEnemy)
{
    if (!DeadEnemy || !IsValid(DeadEnemy))
    {
        UE_LOG(LogTemp, Error, TEXT("[Facade] OnEnemyKilled - DeadEnemy INVALIDO!"));
        return;
    }

    RemainingEnemiesInWave = FMath::Max(0, RemainingEnemiesInWave - 1);
    UE_LOG(LogTemp, Warning, TEXT("[Facade] Enemigo muerto: %s | Restantes en oleada: %d | bIsBossPhase:%d"),
        *DeadEnemy->GetName(), RemainingEnemiesInWave, bIsBossPhase ? 1 : 0);

    if (RemainingEnemiesInWave <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Facade] OLEADA COMPLETADA! Iniciando siguiente..."));
        if (!bIsBossPhase)
        {
            int32 NextWave = CurrentWaveIndex + 1;
            if (NextWave < HordasWaves.Num())
            {
                StartWave(NextWave);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[Facade] TODAS las oleadas de hordas completadas! BossTrigger incoming."));
                OnAllWavesComplete.Broadcast();
            }
        }
    }
}

void ABloodseekerFacade::StartBossPhase()
{
    UE_LOG(LogTemp, Warning, TEXT("[Facade] StartBossPhase - Iniciando oleadas de boss cada 15 segundos"));
    bIsBossPhase = true;
    StartTimerBossWave();
}

void ABloodseekerFacade::StartTimerBossWave()
{
    if (!GetWorld()) return;

    TWeakObjectPtr<ABloodseekerFacade> WeakThis(this);
    GetWorld()->GetTimerManager().SetTimer(
        BossWaveTimerHandle,
        [WeakThis]()
        {
            ABloodseekerFacade* Self = WeakThis.Get();
            if (!Self || !Self->GetWorld()) return;

            int32 RandomIndex = FMath::RandRange(0, Self->BossWaves.Num() - 1);
            const FBloodseekerWaveConfig& Config = Self->BossWaves[RandomIndex];

            Self->RemainingEnemiesInWave = Config.KamikazeCount + Config.LineWelderCount + Config.GravitySiphonCount;

            Self->SpawnEnemiesForConfig(Config);

            if (Self->bIsBossPhase)
            {
                Self->StartTimerBossWave();
            }
        },
        15.0f,
        false
    );
}

void ABloodseekerFacade::PauseBossWaves()
{
    UE_LOG(LogTemp, Warning, TEXT("[Facade] PauseBossWaves - Deteniendo boss waves"));
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(BossWaveTimerHandle);
    }
}

void ABloodseekerFacade::ResumeBossWaves()
{
    UE_LOG(LogTemp, Warning, TEXT("[Facade] ResumeBossWaves - Reanudando boss waves"));
    if (bIsBossPhase)
    {
        StartTimerBossWave();
    }
}

void ABloodseekerFacade::StopAllSpawning()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(BossWaveTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

        for (FTimerHandle& Handle : SpawnTimerHandles)
        {
            GetWorld()->GetTimerManager().ClearTimer(Handle);
        }
        SpawnTimerHandles.Empty();
    }
}

void ABloodseekerFacade::DestroyAllEnemies()
{
    if (!GetWorld()) return;

    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Enemies);

    for (AActor* Enemy : Enemies)
    {
        if (Enemy)
        {
            Enemy->Destroy();
        }
    }
}
