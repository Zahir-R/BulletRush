#include "Enemies/Bloodseeker/LineWelderEnemy.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

ALineWelderEnemy::ALineWelderEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoStartAttack = true;
    AttackInterval = 0.8f;

    Speed = 900.0f;
    StopDistance = 600.0f;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/low-poly-spaceship-110/source/Ship1.Ship1'"));
    if (MeshAsset.Succeeded())
    {
        MeshEnemy->SetStaticMesh(MeshAsset.Object);
        MeshEnemy->SetWorldScale3D(FVector(2.0f));
    }
    //GIRO DE MESH
    MeshRotationOffset = FRotator(0.0f, -90.0f, 0.0f);
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
    // Rotar suavemente hacia el jugador
    RotateTowardsPlayer(DeltaTime);
    
   
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
void ALineWelderEnemy::RotateTowardsPlayer(float DeltaTime)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(
        GetActorLocation(),
        PlayerPawn->GetActorLocation()
    );
    TargetRotation.Pitch = 0.0f;
    TargetRotation.Roll = 0.0f;
    TargetRotation.Yaw += MeshRotationOffset.Yaw;

    FRotator NewRotation = FMath::RInterpTo(
        GetActorRotation(),
        TargetRotation,
        DeltaTime,
        RotationSpeed
    );
    SetActorRotation(NewRotation);
}
void ALineWelderEnemy::Die()
{
    Super::Die();
}
