// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/Orchestrator.h"
#include "Animation/AnimInstance.h"
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
#include "AIController.h"

AOrchestrator::AOrchestrator()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsulaColision"));
	CollisionComponent->InitCapsuleSize(140.f, 40.f);
	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SetRootComponent(CollisionComponent);
	if (MeshEnemy)
	{
		MeshEnemy->DestroyComponent();
		MeshEnemy = nullptr;
	}

	static ConstructorHelpers::FObjectFinder<USkeleton> SkelAsset(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Meshes/Muriel_Skeleton.Muriel_Skeleton"));

	OrchestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Jefe Mesh"));
	OrchestMesh->SetupAttachment(RootComponent);
	OrchestMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));
	OrchestMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

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

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Game/ParagonMuriel/Characters/Heroes/Muriel/Muriel_AnimBlueprint.Muriel_AnimBlueprint_C"));

	if (AnimBP.Succeeded())
	{
		OrchestMesh->SetAnimInstanceClass(AnimBP.Class);
	}

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
	//HealthComp->OnDeath.AddDynamic(this, &AOrchestrator::Die);

	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(120.0f, 20.0f));
	HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Constructor para música...

	BossAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BossAudioComp"));
	BossAudioComp->SetupAttachment(RootComponent);
	BossAudioComp->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> AudioFase1(TEXT("SoundWave'/Game/ParagonMuriel/OrchestratorMusic/Wav/Orchestrator_Normal.Orchestrator_Normal'"));
	if (AudioFase1.Succeeded())
	{
		Phase1Music = AudioFase1.Object;
	}

	// Fase 2
	static ConstructorHelpers::FObjectFinder<USoundBase> AudioFase2(TEXT("SoundWave'/Game/ParagonMuriel/OrchestratorMusic/Wav/Orchestrator_Melancholy.Orchestrator_Melancholy'"));
	if (AudioFase2.Succeeded())
	{
		Phase2Music = AudioFase2.Object;
	}

	// Fase 3
	static ConstructorHelpers::FObjectFinder<USoundBase> AudioFase3(TEXT("SoundWave'/Game/ParagonMuriel/OrchestratorMusic/Wav/Orchestrator_Frenetic.Orchestrator_Frenetic'"));
	if (AudioFase3.Succeeded())
	{
		Phase3Music = AudioFase3.Object;
	}

	// Fase 4
	static ConstructorHelpers::FObjectFinder<USoundBase> AudioFase4(TEXT("SoundWave'/Game/ParagonMuriel/OrchestratorMusic/Wav/Orchestrator_Furious.Orchestrator_Furious'"));
	if (AudioFase4.Succeeded())
	{
		Phase4Music = AudioFase4.Object;
	}

	// AI zona
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->MaxSpeed = 1500.0f; // Flote majestuoso y lento

	MovementComp->NavAgentProps.bCanWalk = true;
	MovementComp->NavAgentProps.bCanFly = true;
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CollisionComponent->SetEnableGravity(false);

	SetActorScale3D(FVector(4.0f));
}

void AOrchestrator::BeginPlay()
{
	Super::BeginPlay();

	if (GetController() == nullptr)
	{
		SpawnDefaultController();
	}

	if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GM->LevelFacade && GM->LevelFacade->bSecretPuzzleSolved)
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
	OrcheDeadState = NewObject<UOrcheDead>(this);
	PhaseTransitionOrcheState = NewObject<UOrchePhaseTransition>(this);

	if (bSecretLevelCleared)
	{
		HealthComp->CurrentHealth = HealthComp->MaxHealth;
		HealthComp->CurrentHealth = HealthComp->MaxHealth * 0.75f;
		ChangeState(Phase2State);
	}
	else
	{
		ChangeState(IntroOrcheState);
	}

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller found"));
		if (APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			AI->SetFocus(Player);
		}
	}
}

void AOrchestrator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		if (APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			AI->SetFocus(Player);
		}
	}
}

float AOrchestrator::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//float DamageTaken = APawn::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("Vida actual del jefe: %f"), HealthComp->CurrentHealth);
	if (DamageAmount <= 0.0f)	return 0.0f;

	//HealthComp->CurrentHealth = HealthComp->CurrentHealth - DamageTaken;

	float HealthPercent = HealthComp->CurrentHealth-DamageAmount / HealthComp->MaxHealth;

	if (HealthPercent <= 0.0f && GetCurrentBossStateName() == "Phase4_Furious")
	{
		UE_LOG(LogTemp, Warning, TEXT("Orchestrator: Jefe cambia a estado muerto..."));
		ChangeState(OrcheDeadState);
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
		ChangeState(PhaseTransitionOrcheState);
	}
	
	HealthComp->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("Vida actual del jefe: %f"), HealthComp->CurrentHealth);
	

	return 0.0f;
}

void AOrchestrator::RoamAroundPlayer()
{
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		if (APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			FVector PlayerLoc = Player->GetActorLocation();

			FVector RandomOffset = FMath::VRand() * FMath::RandRange(800.0f, 1500.0f);

			FVector TargetLocation = PlayerLoc + RandomOffset;
			
			TargetLocation.X = FMath::Clamp(TargetLocation.X, DownLimits.X, UpperLimits.X);
			TargetLocation.Y = FMath::Clamp(TargetLocation.Y, DownLimits.Y, UpperLimits.Y);
			TargetLocation.Z = FMath::Clamp(TargetLocation.Z, DownLimits.Z, UpperLimits.Z);

			// Al ponerlo en FALSE, ignoramos el NavMesh y el jefe vuela libremente en 3D hacia el punto.
			AI->MoveToLocation(TargetLocation, 100.0f, true, false);
		}
	}
}

void AOrchestrator::ErraticTeleport()
{
	if (APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		FVector PlayerLoc = Player->GetActorLocation();
		
		FVector RandomOffset = FMath::VRand() * FMath::RandRange(1200.0f, 2000.0f);


		FVector NewLoc = PlayerLoc + RandomOffset;

		NewLoc.X = FMath::Clamp(NewLoc.X, DownLimits.X, UpperLimits.X);
		NewLoc.Y = FMath::Clamp(NewLoc.Y, DownLimits.Y, UpperLimits.Y);
		NewLoc.Z = FMath::Clamp(NewLoc.Z, DownLimits.Z, UpperLimits.Z);

		SetActorLocation(NewLoc);

		// TODO: Llamar a Niagara para emitir humo/partículas de teletransporte
	}
}