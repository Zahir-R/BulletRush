#include "Enemies/Bloodseeker/KamikazeEnemy.h"
#include "Components/HealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ShapeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AKamikazeEnemy::AKamikazeEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    bAutoStartAttack = false;
    AttackInterval = 0.0f;

    Speed = 600.0f;
    ImpactDamage = 5.0f;

    bHasReachedTarget = false;
}

void AKamikazeEnemy::BeginPlay()
{
    Super::BeginPlay();

    bAutoStartAttack = false;
    StopAttackLoop();

    if (MeshEnemy)
    {
        MeshEnemy->OnComponentBeginOverlap.AddDynamic(this, &AKamikazeEnemy::OnOverlapPlayer);
    }
}

void AKamikazeEnemy::Tick(float DeltaTime)
{
    if (bHasReachedTarget) return;

    Super::Tick(DeltaTime);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    FVector PlayerLoc = PlayerPawn->GetActorLocation();
    FVector CurrentLoc = GetActorLocation();
    FVector DirToPlayer = (PlayerLoc - CurrentLoc).GetSafeNormal();

    float DistToPlayer = FVector::Distance(CurrentLoc, PlayerLoc);

    if (DistToPlayer < 100.0f)
    {
        bHasReachedTarget = true;
        UGameplayStatics::ApplyDamage(
            PlayerPawn, ImpactDamage, GetController(), this, UDamageType::StaticClass()
        );
        Die();
        return;
    }

    FVector NewLocation = CurrentLoc + DirToPlayer * Speed * DeltaTime;

    FRotator LookAt = DirToPlayer.Rotation();
    LookAt.Pitch = 0.0f;
    LookAt.Roll = 0.0f;
    SetActorRotation(LookAt);

    SetActorLocation(NewLocation);
}

void AKamikazeEnemy::InitializeArc(const FVector& BossLocation, const FVector& PlayerLocation)
{
}

void AKamikazeEnemy::OnOverlapPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (bHasReachedTarget) return;

    if (OtherActor && OtherActor != this && OtherActor->ActorHasTag("Player"))
    {
        bHasReachedTarget = true;

        UGameplayStatics::ApplyDamage(
            OtherActor, ImpactDamage, GetController(), this, UDamageType::StaticClass()
        );

        Die();
    }
}

void AKamikazeEnemy::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("[Kamikaze] Die()"));
    bHasReachedTarget = true;
    Super::Die();
}
