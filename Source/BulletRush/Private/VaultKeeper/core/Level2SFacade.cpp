#include "VaultKeeper/core/Level2SFacade.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/enemies/MechaKamikazeEnemy.h"
#include "VaultKeeper/enemies/MechaChargerEnemy.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "Map/LevelPortal.h"
#include "Components/HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Core/BulletRushGameInstance.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ALevel2SFacade::ALevel2SFacade()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ALevel2SFacade::BeginPlay()
{
    Super::BeginPlay();

    Factory = GetWorld()->SpawnActor<AMechaEnemyFactory>(
        AMechaEnemyFactory::StaticClass(),
        FVector::ZeroVector, FRotator::ZeroRotator);

    PortalToBoss = GetWorld()->SpawnActor<ALevelPortal>(
        ALevelPortal::StaticClass(),
        FVector(0.f, 0.f, 0.f), FRotator::ZeroRotator);
	PortalToBoss->SetActorHiddenInGame(true);
}


void ALevel2SFacade::StartLevel()
{
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    

    // Veneno cada segundo
    GetWorld()->GetTimerManager().SetTimer(
        VenomTimer, this, &ALevel2SFacade::ApplyVenom, 1.0f, true);

    SpawnHives();
}


void ALevel2SFacade::SpawnHives()
{
    // Centros de cada colmena separados en el mapa

    TArray<FVector> HiveCenters = {
        FVector(-1200.f,    0.f, 100.f),
        FVector(1200.f,    0.f, 100.f),
        //FVector(0.f, 1200.f, 100.f),
    };

    // Limpiamos antes de llenar
    Hives.Empty();

    for (int32 i = 0; i < HiveCenters.Num(); i++)
    {
        Hives.Add(FHiveData()); // agregamos uno a uno en vez de SetNum
        SpawnHive(i, HiveCenters[i]);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] %d colmenas spawneadas"), Hives.Num());

}

void ALevel2SFacade::SpawnHive(int32 HiveIndex, FVector HiveCenter)
{
    if (!Factory) return;

    FHiveData& Hive = Hives[HiveIndex];

    // Bateria central de la colmena
    Hive.Battery = Cast<ABatteryActor>(
        Factory->CreateEnemy(EMechaEnemyType::BatteryActor,
            HiveCenter, FRotator::ZeroRotator));

    // Offsets relativos al centro de la colmena
    TArray<FVector> DroneOffsets = {
        FVector(200.f,   0.f, 0.f), //FVector(-200.f,   0.f, 0.f),
       // FVector(0.f,   200.f, 0.f), FVector(0.f,  -200.f, 0.f),
        //FVector(300.f, 150.f, 0.f), FVector(-300.f,  150.f, 0.f),
       // FVector(150.f, 300.f, 0.f), FVector(-150.f, -300.f, 0.f),
        //FVector(250.f,-150.f, 0.f), FVector(-250.f, -150.f, 0.f),
    };

    TArray<FVector> ChargerOffsets = {
        FVector(400.f,   0.f, 0.f),
       // FVector(-400.f,  0.f, 0.f),
       // FVector(0.f,   400.f, 0.f),
    };

    TArray<FVector> KamikazeOffsets = {
        FVector(350.f, -350.f, 0.f),
        //FVector(-350.f, 350.f, 0.f),
    };

    // Spawn 10 drones
    for (FVector Offset : DroneOffsets)
    {
        ADronMecha* Drone = Cast<ADronMecha>(
            Factory->CreateEnemy(EMechaEnemyType::DroneMecha,
                HiveCenter + Offset, FRotator::ZeroRotator));
        if (!Drone) continue;
        if (Hive.Battery) Hive.Battery->LinkEnemy(Drone);
        Drone->OnEnemyDeath.AddDynamic(this, &ALevel2SFacade::OnEnemyKilled);
        Hive.Enemies.Add(Drone);
    }

    // Spawn 3 chargers
    for (FVector Offset : ChargerOffsets)
    {
        AMechaChargerEnemy* Charger = Cast<AMechaChargerEnemy>(
            Factory->CreateEnemy(EMechaEnemyType::MechaCharger,
                HiveCenter + Offset, FRotator::ZeroRotator));
        if (!Charger) continue;
        if (Hive.Battery) Hive.Battery->LinkEnemy(Charger);
        Charger->OnEnemyDeath.AddDynamic(this, &ALevel2SFacade::OnEnemyKilled);
        Hive.Enemies.Add(Charger);
    }

    // Spawn 2 kamikazes
    for (FVector Offset : KamikazeOffsets)
    {
        AMechaKamikazeEnemy* Kamikaze = Cast<AMechaKamikazeEnemy>(
            Factory->CreateEnemy(EMechaEnemyType::MechaKamikaze,
                HiveCenter + Offset, FRotator::ZeroRotator));
        if (!Kamikaze) continue;
        if (Hive.Battery) Hive.Battery->LinkEnemy(Kamikaze);
        Kamikaze->OnEnemyDeath.AddDynamic(this, &ALevel2SFacade::OnEnemyKilled);
        Hive.Enemies.Add(Kamikaze);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Colmena %d spawneada: %d enemigos"),
        HiveIndex, Hive.Enemies.Num());
}


void ALevel2SFacade::OnEnemyKilled(AEnemyBase* DeadEnemy)
{
    // Buscamos en qué colmena estaba
    for (FHiveData& Hive : Hives)
    {
        if (Hive.Enemies.Contains(DeadEnemy))
        {
            Hive.Enemies.Remove(DeadEnemy);
            UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Enemigo eliminado. Quedan en colmena: %d"),
                Hive.Enemies.Num());
            break;
        }
    }

    CheckLevelComplete();
}


void ALevel2SFacade::CheckLevelComplete()
{
    // Contamos colmenas limpias
    int32 ClearedCount = 0;
    for (const FHiveData& Hive : Hives)
        if (Hive.IsCleared()) ClearedCount++;

    if (ClearedCount < 2) return;
    if (bLevelComplete) return;

    bLevelComplete = true;
    GetWorld()->GetTimerManager().ClearTimer(VenomTimer);

    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (GI)
    {
        GI->bVaultKeeperWeakened = true;
        GI->Level2State = ELevelState::Boss;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Todas las colmenas limpias — VK debilitado"));
    OpenPortal();
}


void ALevel2SFacade::ApplyVenom()
{
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    if (Player && Player->HealthComp && !Player->HealthComp->bDead)
    {
        Player->HealthComp->CurrentHealth -= VenomDamagePerSecond;

        if (Player->HealthComp->CurrentHealth <= 0.f)
        {
            Player->HealthComp->CurrentHealth = 0.f;
            OnPlayerDeath();
        }
    }
}


void ALevel2SFacade::OnPlayerDeath()
{
    GetWorld()->GetTimerManager().ClearTimer(VenomTimer);
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}


void ALevel2SFacade::OpenPortal()
{
    if (PortalToBoss)
    {
        PortalToBoss->TargetLevelName = FName("Map_02Boss");
        PortalToBoss->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PortalToBoss->SetActorHiddenInGame(false);
    }
    else
        UE_LOG(LogTemp, Error, TEXT("[Level2SFacade] Portal no asignado"));
}