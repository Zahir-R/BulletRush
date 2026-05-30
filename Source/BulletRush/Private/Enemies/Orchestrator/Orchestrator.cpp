// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/Orchestrator.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"

AOrchestrator::AOrchestrator()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsulaColision"));
	CollisionComponent->InitCapsuleSize(88.f, 88.f);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = CollisionComponent;

	static ConstructorHelpers::FObjectFinder<USkeleton> SkelAsset(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_Skeleton.Muriel_Skeleton"));

	OrchestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Jefe Mesh"));
	OrchestMesh->SetupAttachment(RootComponent);
	OrchestMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -44.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_GDC.Muriel_GDC"));
	if (MeshAsset.Succeeded())
	{
		OrchestMesh->SetSkeletalMesh(MeshAsset.Object);
		OrchestMesh->SetCastShadow(false);
		OrchestMesh->SetGenerateOverlapEvents(true);
	}
	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> ExtentsAsset(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_Extents.Muriel_Extents"));
	if (ExtentsAsset.Succeeded())
	{
		OrchestMesh->SetPhysicsAsset(ExtentsAsset.Object);
	}
	OrchestMesh->bCastCapsuleDirectShadow = true;
	OrchestMesh->bCastCapsuleIndirectShadow = true;

	TeamTag = FName("Enemy");
	Tags.Add("Enemigo");
	Tags.Add("Jefe");
}

void AOrchestrator::BeginPlay()
{
	Super::BeginPlay();
}