// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/SecretGuardian.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsAsset.h"

ASecretGuardian::ASecretGuardian()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsulaColision"));
	CollisionComponent->InitCapsuleSize(140.f, 40.f);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = CollisionComponent;

	static ConstructorHelpers::FObjectFinder<USkeleton> SkelAsset(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_Skeleton.Muriel_Skeleton"));

	GuardianMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Jefe Mesh"));
	GuardianMesh->SetupAttachment(RootComponent);
	GuardianMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));
	GuardianMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("SkeletalMesh'/Game/ParagonMuriel/Characters/Heroes/Muriel/Skins/Tier_1/Muriel_Amethyst/Meshes/MurielAmethyst.MurielAmethyst'"));
	if (MeshAsset.Succeeded())
	{
		GuardianMesh->SetSkeletalMesh(MeshAsset.Object);
		GuardianMesh->SetCastShadow(false);
		GuardianMesh->SetGenerateOverlapEvents(true);
	}
	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> ExtentsAsset(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_Extents.Muriel_Extents"));
	if (ExtentsAsset.Succeeded())
	{
		GuardianMesh->SetPhysicsAsset(ExtentsAsset.Object);
	}
	GuardianMesh->bCastCapsuleDirectShadow = true;
	GuardianMesh->bCastCapsuleIndirectShadow = true;

	TeamTag = FName("Enemy");
	Tags.Add("Enemigo");
	Tags.Add("Jefe");
	bAutoStartAttack = true;

	if (!BulletSpawner)
	{
		BulletSpawner = CreateDefaultSubobject<UBulletSpawnerComponent>(TEXT("BulletSpawnerr"));
	}


	if (!HealthComp)
		HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComp->MaxHealth = 500.0f;
	HealthComp->SetInvulnerable(false);

}

void ASecretGuardian::BeginPlay()
{
	Super::BeginPlay();
	AttackInterval = 2.5f;
}

void ASecretGuardian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Hacemos que el guardián mire siempre al jugador suavemente
	if (APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		FVector DirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	
		FRotator TargetRotation = DirectionToPlayer.Rotation();
		TargetRotation.Pitch = 0.0f;
		TargetRotation.Roll = 0.0f;

		// RInterpTo hace que la rotación sea fluida y no robótica
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
		SetActorRotation(NewRotation);
	}
}

void ASecretGuardian::StartAttack()
{
	if (!BulletSpawner) return;

	TArray<FAttackStep> GuardianCombo;

	// EAttackType, Cantidad de balas, Velocidad, Delay, Especial (Ángulo), Daño, Tiempo entre disparos, Escala
	FAttackStep FanStep(EAttackType::Fan, 5, 900.0f, 0.0f, 45.0f, 15.0f, 0.0f, FVector(0.5f));
	FanStep.bUseBossLocation = true; // El origen es el guardián
	GuardianCombo.Add(FanStep);

	FAttackStep BurstStep(EAttackType::Burst, 3, 1200.0f, 0.0f, 0.0f, 20.0f, 0.2f, FVector(0.4f));
	BurstStep.bUseBossLocation = true; // El origen es el guardián
	GuardianCombo.Add(BurstStep);

	BulletSpawner->StartSequence(GuardianCombo);
}