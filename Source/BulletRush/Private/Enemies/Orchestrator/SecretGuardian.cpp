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
}