#include "Enemies/Chronostasis/Boss/SerXBoss.h"
#include "Enemies/Chronostasis/Boss/AlteredZone.h"
#include "Enemies/State/BossStateDead.h"
#include "Components/BossRecorderComponent.h"
#include "Combat/MovementStrategy/SeekMovement.h"
#include "Combat/MovementStrategy/MoveBehindMovement.h"
#include "Combat/MovementStrategy/TriangulationMovement.h"
#include "Combat/MovementStrategy/AscendMovement.h"
#include "Combat/MovementStrategy/DescendMovement.h"
#include "Combat/MovementStrategy/StaticMovement.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
// Subsystem to return active enemy projectiles to pool
#include "Subsystems/ProjectilesSubsystem.h"

ASerXBoss::ASerXBoss()
	: ABossBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("Jefe");

	AttackInterval = 2.5f;

	HealthComp->MaxHealth = 6000.0f;

	AlteredZoneClass = AAlteredZone::StaticClass();

	RecorderComponent = CreateDefaultSubobject<UBossRecorderComponent>(TEXT("RecorderComponent"));

	MovementStrategy = CreateDefaultSubobject<USeekMovement>(TEXT("SeekMovement"));
	SeekStrat = Cast<USeekMovement>(MovementStrategy);
	if (SeekStrat) SeekStrat->Speed = 4000.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/ChronoEnemies/Boss/Nebula_Voyager_boss_texture.Nebula_Voyager_boss_texture'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);

	MeshEnemy->SetRelativeScale3D(FVector(7.5f, 7.5f, 7.5f));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/ChronoEnemies/Boss/M_Nebula_Voyager_boss.M_Nebula_Voyager_boss'"));
	if (MaterialAsset.Succeeded()) MeshEnemy->SetMaterial(0, MaterialAsset.Object);
}

void ASerXBoss::BeginPlay()
{
	Super::BeginPlay();

	if (bIsClone)
	{
		HealthComp->CurrentHealth = HealthComp->MaxHealth;
		HealthComp->SetInvulnerable(false);
		StopAttackLoop();
		if (CurrentStateObject)
		{
			CurrentStateObject->ExitState(this);
			CurrentStateObject = nullptr;
		}
		GetWorldTimerManager().ClearTimer(IntroTimer);
		SetupAttackCombos();
		SetupMovementStrategies();
		return;
	}

	HealthComp->CurrentHealth = HealthComp->MaxHealth;

	SetupAttackCombos();
	SetupMovementStrategies();

	GetWorldTimerManager().SetTimer(LinkerSpawnTimerHandle, this, &ASerXBoss::OnLinkerSpawnTimer, LinkerSpawnInterval, true);
	GetWorldTimerManager().SetTimer(ZoneSpawnTimerHandle, this, &ASerXBoss::OnZoneSpawnTimer, ZoneSpawnInterval, true);
}

void ASerXBoss::Tick(float DeltaTime)
{
	if (bIsClone)
	{
		AEnemyBase::Tick(DeltaTime);
	}
	else
	{
		ABossBase::Tick(DeltaTime);
	}

	if (bExecutingAttackCombo && !bIsDead)
	{
		AttackComboElapsed += DeltaTime;
		if (AttackComboElapsed >= AttackInterval)
		{
			AdvanceAttackCombo();
			AttackComboElapsed = 0.f;
		}
	}

	if (bExecutingMovementCombo && !bIsDead)
	{
		MovementStepElapsed += DeltaTime;

		if (MoveCombos.IsValidIndex(CurrentMovementCombo) && CurrentMovementStep < MoveCombos[CurrentMovementCombo].Num())
		{
			const FMovementComboStep& Step = MoveCombos[CurrentMovementCombo][CurrentMovementStep];

			bool bStepDone = false;
			if (Step.Duration > 0.f)
			{
				bStepDone = MovementStepElapsed >= Step.Duration;
			}
			else if (MovementStrategy)
			{
				bStepDone = MovementStrategy->bCompleted;
			}

			if (bStepDone)
			{
				AdvanceMovementCombo();
			}
		}
	}

	if (bIsMoving && MovementStrategy && !bIsDead)
	{
		FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaTime, MovementTarget);
		NewPos = ApplyEnemySeparation(NewPos);
		SetActorLocation(NewPos);
	}
	Super::Tick(DeltaTime);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
		TargetRotation.Roll = 0.0f;
		TargetRotation.Yaw += 180.0f;
		TargetRotation.Pitch *= -1.0f;

		FRotator CurrRotation = GetActorRotation();
		FRotator SmoothRotation = UKismetMathLibrary::RInterpTo(CurrRotation, TargetRotation, DeltaTime, RotationSpeed);
		SetActorRotation(SmoothRotation);
	}

}

void ASerXBoss::BeginDestroy()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LinkerSpawnTimerHandle);
		World->GetTimerManager().ClearTimer(ZoneSpawnTimerHandle);
	}
	Super::BeginDestroy();
}

void ASerXBoss::SetupMovementStrategies()
{
	if (!MoveBehindStrat)
	{
		MoveBehindStrat = NewObject<UMoveBehindMovement>(this);
		MoveBehindStrat->Speed = 5000.f;
		MoveBehindStrat->Distance = 2500.f;
	}
	if (!TriangulationStrat)
	{
		TriangulationStrat = NewObject<UTriangulationMovement>(this);
		TriangulationStrat->Speed = 5000.f;
		TriangulationStrat->TriangleSize = 2000.f;
	}
	if (!AscendStrat)
	{
		AscendStrat = NewObject<UAscendMovement>(this);
		AscendStrat->Speed = 4000.f;
		AscendStrat->MaxDelta = 1200.f;
	}
	if (!DescendStrat)
	{
		DescendStrat = NewObject<UDescendMovement>(this);
		DescendStrat->Speed = 4000.f;
		DescendStrat->MaxDelta = 1200.f;
	}
	if (!StaticStrat)
	{
		StaticStrat = NewObject<UStaticMovement>(this);
	}
}

void ASerXBoss::ExecuteMovement(int32 StrategyIndex, FVector Target)
{
	CurrentStrategyIndex = StrategyIndex;
	MovementTarget = Target;
	bIsMoving = true;

	UMovementStrat* NewStrat = nullptr;
	switch (StrategyIndex)
	{
	case 0: NewStrat = SeekStrat; break;
	case 1: NewStrat = MoveBehindStrat; break;
	case 2: NewStrat = TriangulationStrat; break;
	case 3: NewStrat = AscendStrat; break;
	case 4: NewStrat = DescendStrat; break;
	case 5: NewStrat = StaticStrat; break;
	}

	if (NewStrat)
	{
		NewStrat->Reset();
		MovementStrategy = NewStrat;
	}
}

void ASerXBoss::SetupAttackCombos()
{
	AttackCombos.Empty();
	MoveCombos.Empty();

	// Combo 0: Circle, Spiral, Circle, Circle, Sphere, Spiral
	{
		TArray<FAttackStep> Steps;
		Steps.Reserve(6);
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		Steps.Add(FAttackStep(EAttackType::Spiral, 30, 400.f, AttackInterval, 15.f, 18.f));
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::Spiral, 30, 400.f, AttackInterval, 15.f, 18.f));
		AttackCombos.Add(MoveTemp(Steps));
	}

	// Combo 1: Spiral, Sphere, Sphere, Sphere, Circle, Spiral, Circle
	{
		TArray<FAttackStep> Steps;
		Steps.Reserve(7);
		Steps.Add(FAttackStep(EAttackType::Spiral, 30, 400.f, AttackInterval, 15.f, 18.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		Steps.Add(FAttackStep(EAttackType::Spiral, 30, 400.f, AttackInterval, 15.f, 18.f));
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		AttackCombos.Add(MoveTemp(Steps));
	}

	// Combo 2: Surround, Circle, Sphere, Sphere, Surround, Surround
	{
		TArray<FAttackStep> Steps;
		Steps.Reserve(6);
		Steps.Add(FAttackStep(EAttackType::SurroundingBullets, 30, 0.f, AttackInterval, 0.f, 0));
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::SurroundingBullets, 30, 0.f, AttackInterval, 0.f, 0));
		Steps.Add(FAttackStep(EAttackType::SurroundingBullets, 30, 0.f, AttackInterval, 0.f, 0));
		AttackCombos.Add(MoveTemp(Steps));
	}

	// Combo 3: Circle, Circle, Spiral, Sphere, Sphere, Surround
	{
		TArray<FAttackStep> Steps;
		Steps.Reserve(6);
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		Steps.Add(FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f));
		Steps.Add(FAttackStep(EAttackType::Spiral, 30, 400.f, AttackInterval, 15.f, 18.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f));
		Steps.Add(FAttackStep(EAttackType::SurroundingBullets, 30, 0.f, AttackInterval, 0.f, 0));
		AttackCombos.Add(MoveTemp(Steps));
	}

	// MoveCombo 0: Static(3s), Static(3s), Seek(2s), Triangulation(complete), Static(3s)
	{
		TArray<FMovementComboStep> Steps;
		Steps.Reserve(5);
		Steps.Add({ 5, 1.5f });
		Steps.Add({ 5, 1.5f });
		Steps.Add({ 2, 0.0f });
		Steps.Add({ 2, 0.0f });
		Steps.Add({ 5, 1.5f });
		MoveCombos.Add(MoveTemp(Steps));
	}

	// MoveCombo 1: Static(3s), Triangulation(complete), Static(3s), Ascend(2s), Descend(2s)
	{
		TArray<FMovementComboStep> Steps;
		Steps.Reserve(5);
		Steps.Add({ 5, 1.5f });
		Steps.Add({ 2, 0.0f });
		Steps.Add({ 5, 0.5f });
		Steps.Add({ 3, 2.0f });
		Steps.Add({ 4, 2.0f });
		MoveCombos.Add(MoveTemp(Steps));
	}

	// MoveCombo 2: Descend(2s), MoveBehind(complete), Static(3s), MoveBehind(complete), Static(3s)
	{
		TArray<FMovementComboStep> Steps;
		Steps.Reserve(5);
		Steps.Add({ 4, 2.f });
		Steps.Add({ 1, 0.f });
		Steps.Add({ 5, 0.5f });
		Steps.Add({ 1, 0.f });
		Steps.Add({ 5, 3.f });
		MoveCombos.Add(MoveTemp(Steps));
	}
}

void ASerXBoss::Attack()
{
	if (bExecutingAttackCombo || !BulletSpawner || bIsDead) return;

	int32 ComboIndex = FMath::RandRange(0, 3);
	StartAttackCombo(ComboIndex);

	int32 MovChoice = FMath::RandRange(0, 2);
	StartMovementCombo(MovChoice);
}

void ASerXBoss::StartAttackCombo(int32 ComboIndex)
{
	if (!AttackCombos.IsValidIndex(ComboIndex) || AttackCombos[ComboIndex].Num() == 0) return;

	CurrentAttackCombo = ComboIndex;
	CurrentAttackStep = 0;
	AttackComboElapsed = 0.f;
	bExecutingAttackCombo = true;

	FireAttackStep(AttackCombos[ComboIndex][0]);

	if (AttackCombos[ComboIndex].Num() == 1)
	{
		bExecutingAttackCombo = false;
	}
}

void ASerXBoss::AdvanceAttackCombo()
{
	CurrentAttackStep++;

	if (!AttackCombos.IsValidIndex(CurrentAttackCombo)) return;

	if (CurrentAttackStep < AttackCombos[CurrentAttackCombo].Num())
	{
		FireAttackStep(AttackCombos[CurrentAttackCombo][CurrentAttackStep]);
	}
	else
	{
		bExecutingAttackCombo = false;
	}
}

void ASerXBoss::FireAttackStep(const FAttackStep& Step)
{
	if (!BulletSpawner || bIsDead) return;

	TArray<FAttackStep> Single = { Step };
	BulletSpawner->StartSequence(Single);

	if (RecorderComponent)
	{
		int32 PatternIndex = 0;
		switch (Step.Type)
		{
		case EAttackType::Circle: PatternIndex = 0; break;
		case EAttackType::Sphere: PatternIndex = 1; break;
		case EAttackType::Spiral: PatternIndex = 2; break;
		case EAttackType::SurroundingBullets: PatternIndex = 3; break;
		}
		RecorderComponent->RecordAttack(PatternIndex);
	}

	if (!bIsClone)
	{
		StepsSinceMinionSpawn++;
		if (StepsSinceMinionSpawn >= 3)
		{
			StepsSinceMinionSpawn = 0;
			SpawnRandomMinion();
			SpawnRandomMinion();
		}
	}
}

void ASerXBoss::StartMovementCombo(int32 ComboIndex)
{
	if (!MoveCombos.IsValidIndex(ComboIndex) || MoveCombos[ComboIndex].Num() == 0) return;

	if (MovementStrategy)
		MovementStrategy->Reset();
	bIsMoving = false;

	CurrentMovementCombo = ComboIndex;
	CurrentMovementStep = 0;
	MovementStepElapsed = 0.f;
	bExecutingMovementCombo = true;

	StartMovementStep(0);
}

void ASerXBoss::AdvanceMovementCombo()
{
	if (MovementStrategy)
		MovementStrategy->Reset();

	CurrentMovementStep++;

	if (MoveCombos.IsValidIndex(CurrentMovementCombo) && CurrentMovementStep < MoveCombos[CurrentMovementCombo].Num())
	{
		StartMovementStep(CurrentMovementStep);
	}
	else
	{
		bExecutingMovementCombo = false;
		bIsMoving = false;
	}
}

void ASerXBoss::StartMovementStep(int32 StepIndex)
{
	if (!MoveCombos.IsValidIndex(CurrentMovementCombo)) return;
	if (!MoveCombos[CurrentMovementCombo].IsValidIndex(StepIndex)) return;

	if (RecorderComponent)
	{
		RecorderComponent->OnMovementStepFinished();
	}

	const FMovementComboStep& Step = MoveCombos[CurrentMovementCombo][StepIndex];
	MovementStepElapsed = 0.f;

	if (Step.StrategyIndex == 5)
	{
		bIsMoving = false;
		return;
	}

	FVector Target = GetMovementTargetForStrategy(Step.StrategyIndex);
	MoveTo(Target, Step.StrategyIndex);
}

FVector ASerXBoss::GetMovementTargetForStrategy(int32 StrategyIndex)
{
	APawn* PlayerPawn = GetWorld() ? UGameplayStatics::GetPlayerPawn(GetWorld(), 0) : nullptr;
	if (!PlayerPawn) return GetActorLocation();

	switch (StrategyIndex)
	{
	case 0:
		return PlayerPawn->GetActorLocation();

	case 1:
	{
		FVector PlayerLoc = PlayerPawn->GetActorLocation();
		FVector PlayerForward = PlayerPawn->GetActorForwardVector();
		return PlayerLoc - PlayerForward * 200.f;
	}

	default:
		return GetActorLocation();
	}
}

void ASerXBoss::ExecuteAttack(int32 PatternIndex)
{
	if (!BulletSpawner || bIsDead || PatternIndex < 0 || PatternIndex > 3) return;

	FAttackStep Step;
	if (AttackIdentifier >= 1)
	{
		switch (PatternIndex)
		{
		case 0: Step = FAttackStep(EAttackType::Circle, 24, 600.f, 1.2f, 0.f, 12.f); break;
		case 1: Step = FAttackStep(EAttackType::Sphere, 80, 800.f, 1.6f, 0.f, 10.f); break;
		case 2: Step = FAttackStep(EAttackType::Spiral, 40, 500.f, 1.5f, 12.f, 15.f); break;
		case 3: Step = FAttackStep(EAttackType::SurroundingBullets, 40, 0.f, 1.8f, 0.f, 0.f); break;
		}
	}
	else
	{
		switch (PatternIndex)
		{
		case 0: Step = FAttackStep(EAttackType::Circle, 16, 500.f, AttackInterval, 0.f, 15.f); break;
		case 1: Step = FAttackStep(EAttackType::Sphere, 60, 700.f, AttackInterval, 0.f, 12.f); break;
		case 2: Step = FAttackStep(EAttackType::Spiral, 30, 400.f, AttackInterval, 15.f, 18.f); break;
		case 3: Step = FAttackStep(EAttackType::SurroundingBullets, 30, 0.f, AttackInterval, 0.f, 0); break;
		}
	}

	BulletSpawner->StartSequence({ Step });
}

void ASerXBoss::DoSpawnLinker()
{
	if (!LinkerFactory || !GetWorld() || bIsDead) return;
	if (ActiveLinkerCount >= MaxLinkers) return;

	FVector SpawnLoc = GetActorLocation() + FMath::VRand() * 350.f;
	SpawnLoc.Z = GetActorLocation().Z;
	AEnemyBase* Linker = LinkerFactory->CreateEnemy(GetWorld(), SpawnLoc);
	if (Linker)
	{
		ActiveLinkerCount++;
		Linker->OnEnemyDeath.AddDynamic(this, &ASerXBoss::OnLinkerDied);
	}
}

void ASerXBoss::SpawnRandomMinion()
{
	if (MinionFactories.Num() == 0 || !GetWorld() || bIsDead) return;

	int32 Idx = FMath::RandRange(0, MinionFactories.Num() - 1);
	FVector SpawnLoc = GetActorLocation() + FMath::VRand() * 300.f;
	SpawnLoc.Z = GetActorLocation().Z;
	MinionFactories[Idx]->CreateEnemy(GetWorld(), SpawnLoc);
}

void ASerXBoss::ActivateZone()
{
	if (!AlteredZoneClass || !GetWorld() || bIsDead) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector ZoneLoc = PC && PC->GetPawn() ? PC->GetPawn()->GetActorLocation() : GetActorLocation();
	ZoneLoc.Z += 50.f;

	GetWorld()->SpawnActor<AAlteredZone>(AlteredZoneClass, ZoneLoc, FRotator::ZeroRotator, Params);
}

void ASerXBoss::MoveTo(FVector Target, int32 StrategyIndex)
{
	if (bIsDead) return;

	ExecuteMovement(StrategyIndex, Target);

	if (RecorderComponent)
	{
		RecorderComponent->RecordMove(Target, StrategyIndex);
	}
}

void ASerXBoss::OnLinkerSpawnTimer()
{
	DoSpawnLinker();
}

void ASerXBoss::OnZoneSpawnTimer()
{
	ActivateZone();
	if (!bIsClone && RecorderComponent)
	{
		RecorderComponent->TryStartRecording(AttackIdentifier);
		UE_LOG(LogTemp, Warning, TEXT("Boss should have started recording"));
	}
}

void ASerXBoss::SetLinkerFactory(UChronostasisFactoryEnemy* Factory)
{
	LinkerFactory = Factory;
}

void ASerXBoss::AddMinionFactory(UChronostasisFactoryEnemy* Factory)
{
	if (Factory)
	{
		MinionFactories.Add(Factory);
	}
}

void ASerXBoss::OnLinkerDied(AEnemyBase* DeadLinker)
{
	if (DeadLinker)
	{
		ActiveLinkerCount = FMath::Max(0, ActiveLinkerCount - 1);
	}
}

float ASerXBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsClone) return AEnemyBase::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return ABossBase::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ASerXBoss::Die()
{
	if (bIsDead) return;
	if (bIsClone) {
		AEnemyBase::Die();
		return;
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LinkerSpawnTimerHandle);
		World->GetTimerManager().ClearTimer(ZoneSpawnTimerHandle);
	}

	Super::Die();

	for (TActorIterator<AEnemyBase> It(GetWorld()); It; ++It)
	{
		AEnemyBase* Enemy = *It;
		if (Enemy && Enemy != this)
		{
			Enemy->Die();
		}
	}
}
