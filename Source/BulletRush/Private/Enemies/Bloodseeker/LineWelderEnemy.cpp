#include "Enemies/Bloodseeker/LineWelderEnemy.h"
#include "Combat/MovementStrategy/SinusoidalSeekMovement.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ALineWelderEnemy::ALineWelderEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoStartAttack = true;
    AttackInterval = 1.5f;

    MovementStrategy = CreateDefaultSubobject<USinusoidalSeekMovement>(TEXT("SinusoidalMovement"));
}

void ALineWelderEnemy::BeginPlay()
{
    Super::BeginPlay();

    USinusoidalSeekMovement* SinMovement = Cast<USinusoidalSeekMovement>(MovementStrategy);
    if (SinMovement)
    {
        SinMovement->Amplitude = 150.0f;
        SinMovement->Frequency = 2.0f;
        SinMovement->StopDistance = 600.0f;
        SinMovement->InitialPhase = FMath::FRandRange(0.0f, PI * 2.0f);
    }

    if (MeshEnemy)
    {
        MeshEnemy->OnComponentBeginOverlap.AddDynamic(this, &ALineWelderEnemy::OnOverlapPlayer);
    }
}

void ALineWelderEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (MovementStrategy)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            FVector Target = PlayerPawn->GetActorLocation();
            FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaTime, Target);
            SetActorLocation(NewPos);
        }
    }
}

void ALineWelderEnemy::StartAttack()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    FVector Dir = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    if (BulletSpawner)
    {
        BulletSpawner->InternalSpawn(GetActorLocation(), Dir, 2000.0f, 10.0f);
    }
}

void ALineWelderEnemy::OnOverlapPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Player"))
    {
        UGameplayStatics::ApplyDamage(
            OtherActor,
            10.0f,
            GetController(),
            this,
            UDamageType::StaticClass()
        );
    }
}

void ALineWelderEnemy::Die()
{
    Super::Die();
}
