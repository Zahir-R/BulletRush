// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/Orchestrator.h"
#include "Enemies/State/BossStateBase.h"
#include "Enemies/State/BossStateDead.h"
#include "Enemies/Orchestrator/OrchestratorStates.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/RhytmConductorComponent.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsAsset.h"

AOrchestrator::AOrchestrator()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsulaColision"));
	CollisionComponent->InitCapsuleSize(140.f, 40.f);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = CollisionComponent;

	static ConstructorHelpers::FObjectFinder<USkeleton> SkelAsset(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_Skeleton.Muriel_Skeleton"));

	OrchestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Jefe Mesh"));
	OrchestMesh->SetupAttachment(RootComponent);
	OrchestMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));

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

	RhythmConductor = CreateDefaultSubobject<URhytmConductorComponent>(TEXT("RhythmConductor"));
	bSecretLevelCleared = false;
	CurrentStateObject = nullptr;
	bAutoStartAttack = false;

	if (!BulletSpawner)
	{
		BulletSpawner = CreateDefaultSubobject<UBulletSpawnerComponent>(TEXT("BulletSpawnerr"));
	}


	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComp->MaxHealth = 4000.0f;
	HealthComp->SetInvulnerable(false);
}

void AOrchestrator::BeginPlay()
{
	Super::BeginPlay();

	if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GM->LevelFacade && GM->LevelFacade->bSecretPuzzleSolved) // Asegúrate de hacer pública bSecretPuzzleSolved en la fachada
		{
			bSecretLevelCleared = true;
		}
	}
	HealthComp->CurrentHealth = HealthComp->MaxHealth;
	Phase1State = NewObject<UOrchestrator_Normal>(this);
	Phase2State = NewObject<UOrchestrator_Melancholy>(this);
	Phase3State = NewObject<UOrchestrator_Frenetic>(this);
	Phase4State = NewObject<UOrchestrator_Furious>(this);
	IntroOrcheState = NewObject<UOrchestratorIntro>(this);
	PhaseTransitionOrcheState = NewObject<UOrchePhaseTransition>(this);

	if (bSecretLevelCleared)
	{
		HealthComp->CurrentHealth = HealthComp->MaxHealth * 0.75f;
		ChangeState(Phase2State);
	}
	else
	{
		ChangeState(IntroOrcheState);
	}
}

float AOrchestrator::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//float DamageTaken = APawn::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("Vida actual del jefe: %f"), HealthComp->CurrentHealth);
	if (DamageAmount <= 0.0f)	return 0.0f;

	//HealthComp->CurrentHealth = HealthComp->CurrentHealth - DamageTaken;
	HealthComp->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("Vida actual del jefe: %f"), HealthComp->CurrentHealth);

	float HealthPercent = HealthComp->CurrentHealth / HealthComp->MaxHealth;

	if (HealthPercent <= 0.0f && GetCurrentBossStateName() == "Phase4_Furious")
	{
		ChangeState(DeadState);
	}
	else if (HealthPercent <= 0.25 && GetCurrentBossStateName() == "Phase3_Frenetic")
	{
		ChangeState(PhaseTransitionOrcheState);
	}
	else if (HealthPercent <= 0.50f && GetCurrentBossStateName() == "Phase2_Melancholy")
	{
		ChangeState(PhaseTransitionOrcheState);
	}
	else if (HealthPercent <= 0.75f && GetCurrentBossStateName() == "Phase1_Normal")
	{
		ChangeState(PhaseTransitionOrcheState); // Luego de la transición, ir a Phase 2
	}
	
	
	

	return 0.0f;
}