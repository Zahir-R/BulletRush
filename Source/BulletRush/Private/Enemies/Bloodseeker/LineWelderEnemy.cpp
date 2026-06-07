#include "Enemies/Bloodseeker/LineWelderEnemy.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ALineWelderEnemy::ALineWelderEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoStartAttack = true;
    AttackInterval = 1.5f;
}

void ALineWelderEnemy::BeginPlay()
{
    Super::BeginPlay();

    if (MeshEnemy)
    {
        MeshEnemy->OnComponentBeginOverlap.AddDynamic(this, &ALineWelderEnemy::OnOverlapPlayer);
    }
}

void ALineWelderEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = PlayerPawn->GetActorLocation();
    FVector Dir = (TargetLoc - MyLoc);
    float Distance = Dir.Size();

    if (Distance > StopDistance)
    {
        Dir.Normalize();
        SetActorLocation(MyLoc + Dir * Speed * DeltaTime);
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
