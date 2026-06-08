#include "VaultKeeper/enemies/MechaChargerEnemy.h"
#include "Combat/MovementStrategy/SeekMovement.h"
#include "Components/SphereComponent.h"
#include "Components/BuffComponent.h"
#include "Buffs/PlayerSlowDecorator.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AMechaChargerEnemy::AMechaChargerEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoStartAttack = false;
    AttackInterval = 0.0f;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/AccuCities/meshes/Meshy_AI_Iron_Spiked_Sphere_0607054312_texture.Meshy_AI_Iron_Spiked_Sphere_0607054312_texture'"));
    if(MeshAsset.Succeeded())
    {
        MeshEnemy->SetStaticMesh(MeshAsset.Object);
        
	}

    // SeekMovement: persigue al jugador directamente
    MovementStrategy = CreateDefaultSubobject<USeekMovement>(TEXT("SeekMovement"));
    USeekMovement* Seek = Cast<USeekMovement>(MovementStrategy);
    if (Seek) Seek->Speed = ChargeSpeed;

    // Esfera de colisión para detectar impacto con el jugador
    HitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HitCollision"));
    HitCollision->InitSphereRadius(80.0f);
    HitCollision->SetupAttachment(RootComponent);
    HitCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AMechaChargerEnemy::BeginPlay()
{
    Super::BeginPlay();

    // Sincronizamos la velocidad por si se cambió en Blueprint
    USeekMovement* Seek = Cast<USeekMovement>(MovementStrategy);
    if (Seek) Seek->Speed = ChargeSpeed;

    HitCollision->OnComponentBeginOverlap.AddDynamic(
        this, &AMechaChargerEnemy::OnHitPlayer);
}

void AMechaChargerEnemy::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!MovementStrategy) return;

    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    FVector NewPos = MovementStrategy->GetNextPosition(
        this, DeltaSeconds, Player->GetActorLocation());

    NewPos = ApplyEnemySeparation(NewPos);
    SetActorLocation(NewPos);
}

void AMechaChargerEnemy::OnHitPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor != Player) return;

    // Aplicar slow usando el BuffComponent del jugador
    UBuffComponent* Buff = Player->FindComponentByClass<UBuffComponent>();
    if (Buff)
    {
        Buff->ApplyBuff(UPlayerSlowDecorator::StaticClass(), SlowDuration, 0.0f);
        UE_LOG(LogTemp, Warning,
            TEXT("[MechaCharger] Impacto — slow aplicado por %.1fs"), SlowDuration);
    }

    Die();
}