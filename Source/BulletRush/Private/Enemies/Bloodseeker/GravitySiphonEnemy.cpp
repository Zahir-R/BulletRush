#include "Enemies/Bloodseeker/GravitySiphonEnemy.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AGravitySiphonEnemy::AGravitySiphonEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/22_STATION/2_Stacioin_Asset.2_Stacioin_Asset'"));
    if (MeshAsset.Succeeded()) {
        MeshEnemy->SetStaticMesh(MeshAsset.Object);
        MeshEnemy->SetWorldScale3D(FVector(0.9f));
    }
    bAutoStartAttack = true;
    AttackInterval = 2.0f;

    PullForce = 150.0f;
    DamagePerSecond = 15.0f;
    StillnessThreshold = 50.0f;
    ActivationRadius = 800.0f;

    SuctionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("SuctionVolume"));
    SuctionVolume->SetupAttachment(RootComponent);
    SuctionVolume->SetSphereRadius(ActivationRadius);
    SuctionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGravitySiphonEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void AGravitySiphonEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    float Distance = FVector::Distance(GetActorLocation(), PlayerPawn->GetActorLocation());

    DrawDebugSphere(GetWorld(), GetActorLocation(), ActivationRadius, 16, FColor::Red, false, -1.0f, 0, 1.0f);

    if (Distance > ActivationRadius) return;

    FVector DirectionToSiphon = (GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
    FVector PullVector = DirectionToSiphon * PullForce * DeltaTime;

    PlayerPawn->AddMovementInput(DirectionToSiphon, PullForce * 0.05f);

    float PlayerSpeed = PlayerPawn->GetVelocity().Size();
    if (PlayerSpeed < StillnessThreshold)
    {
        UGameplayStatics::ApplyDamage(
            PlayerPawn,
            DamagePerSecond * DeltaTime,
            nullptr,
            this,
            UDamageType::StaticClass()
        );
    }
}

void AGravitySiphonEnemy::StartAttack()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    FVector Dir = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    if (BulletSpawner)
    {
        BulletSpawner->InternalSpawn(GetActorLocation(), Dir, 2000.0f, 10.0f);
    }
}
