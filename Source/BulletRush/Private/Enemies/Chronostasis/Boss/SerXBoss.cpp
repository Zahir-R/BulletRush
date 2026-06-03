#include "Enemies/Chronostasis/Boss/SerXBoss.h"
#include "Enemies/Chronostasis/Boss/AlteredZone.h"
#include "Enemies/State/BossStateDead.h"
#include "Combat/Commands/AttackCommand.h"
#include "Combat/Commands/MoveCommand.h"
#include "Combat/MovementStrategy/SeekMovement.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ASerXBoss::ASerXBoss()
	: ABossBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add("Jefe");

	AttackInterval = 2.5f;

	HealthComp->MaxHealth = 6000.0f;

	AlteredZoneClass = AAlteredZone::StaticClass();

	MovementStrategy = CreateDefaultSubobject<USeekMovement>(TEXT("SeekMovement"));
<<<<<<< Updated upstream
	USeekMovement* Seek = Cast<USeekMovement>(MovementStrategy);
	if (Seek) Seek->Speed = 400.f;
=======
	SeekStrat = Cast<USeekMovement>(MovementStrategy);
	if (SeekStrat) SeekStrat->Speed = 4000.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);
>>>>>>> Stashed changes
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
		SetupPhase2Combos();
		return;
	}

	HealthComp->CurrentHealth = HealthComp->MaxHealth;

	SetupAttackCombos();
	SetupPhase2Combos();

	GetWorldTimerManager().SetTimer(LinkerSpawnTimerHandle, this, &ASerXBoss::OnLinkerSpawnTimer, LinkerSpawnInterval, true);
	GetWorldTimerManager().SetTimer(ZoneSpawnTimerHandle, this, &ASerXBoss::OnZoneSpawnTimer, ZoneSpawnInterval, true);
}

void ASerXBoss::Tick(float DeltaTime)
{
	if (bIsClone)
	{
		AEnemyBase::Tick(DeltaTime);
		return;
	}

	ABossBase::Tick(DeltaTime);

	if (bIsMoving && MovementStrategy && !bIsDead)
	{
		FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaTime, MovementTarget);
		NewPos = ApplyEnemySeparation(NewPos);
		SetActorLocation(NewPos);
		if (FVector::Dist(NewPos, MovementTarget) < 50.f)
		{
			bIsMoving = false;
		}
	}
}

void ASerXBoss::BeginDestroy()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LinkerSpawnTimerHandle);
		World->GetTimerManager().ClearTimer(ZoneSpawnTimerHandle);
		World->GetTimerManager().ClearTimer(RecordingTimerHandle);
	}
	Super::BeginDestroy();
}

<<<<<<< Updated upstream
=======
void ASerXBoss::SetupMovementStrategies()
{
	if (!MoveBehindStrat)
	{
		MoveBehindStrat = NewObject<UMoveBehindMovement>(this);
		MoveBehindStrat->Speed = 3000.f;
		MoveBehindStrat->Distance = 200.f;
	}
	if (!TriangulationStrat)
	{
		TriangulationStrat = NewObject<UTriangulationMovement>(this);
		TriangulationStrat->Speed = 3000.f;
		TriangulationStrat->TriangleSize = 1000.f;
	}
	if (!AscendStrat)
	{
		AscendStrat = NewObject<UAscendMovement>(this);
		AscendStrat->Speed = 4000.f;
		AscendStrat->MaxDelta = 700.f;
	}
	if (!DescendStrat)
	{
		DescendStrat = NewObject<UDescendMovement>(this);
		DescendStrat->Speed = 4000.f;
		DescendStrat->MaxDelta = 700.f;
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

>>>>>>> Stashed changes
void ASerXBoss::SetupAttackCombos()
{
	CircleCombo.Empty();
	CircleCombo.Add(FAttackStep(EAttackType::Circle, 16, 500.f, 1.5f, 0.f, 15.f));

	SphereCombo.Empty();
	SphereCombo.Add(FAttackStep(EAttackType::Sphere, 60, 700.f, 2.0f, 0.f, 12.f));

	SpiralCombo.Empty();
	SpiralCombo.Add(FAttackStep(EAttackType::Spiral, 30, 400.f, 2.0f, 15.f, 18.f));

	SurroundCombo.Empty();
	SurroundCombo.Add(FAttackStep(EAttackType::SurroundingBullets, 30, 0.f, 2.5f, 0.f, 0));
}

void ASerXBoss::SetupPhase2Combos()
{
	CircleCombo2.Empty();
	CircleCombo2.Add(FAttackStep(EAttackType::Circle, 24, 600.f, 1.2f, 0.f, 12.f));

<<<<<<< Updated upstream
	SphereCombo2.Empty();
	SphereCombo2.Add(FAttackStep(EAttackType::Sphere, 80, 800.f, 1.6f, 0.f, 10.f));

	SpiralCombo2.Empty();
	SpiralCombo2.Add(FAttackStep(EAttackType::Spiral, 40, 500.f, 1.5f, 12.f, 15.f));

	SurroundCombo2.Empty();
	SurroundCombo2.Add(FAttackStep(EAttackType::SurroundingBullets, 40, 0.f, 1.8f, 0.f, 0.f));
=======
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
>>>>>>> Stashed changes
}

void ASerXBoss::Attack()
{
	if (!BulletSpawner || bIsDead) return;

<<<<<<< Updated upstream
	AttackCount++;
	AttacksSinceChargerSpawn++;

	int32 PatternIndex = FMath::RandRange(0, 3);
	ExecuteAttack(PatternIndex);

	if (bIsRecording)
	{
		UAttackCommand* Cmd = NewObject<UAttackCommand>(this);
		Cmd->AttackPatternIndex = PatternIndex;
		RecordCommand(Cmd);
	}

	if (AttacksSinceChargerSpawn >= 3 && !bIsClone)
	{
		AttacksSinceChargerSpawn = 0;
		DoSpawnCharger();
	}
=======
	int32 ComboIndex = FMath::RandRange(0, 3);
	StartAttackCombo(ComboIndex);

	int32 MovChoice = FMath::RandRange(0, 2);
	StartMovementCombo(MovChoice);
>>>>>>> Stashed changes
}

void ASerXBoss::ExecuteAttack(int32 PatternIndex)
{
	if (!BulletSpawner || bIsDead) return;

<<<<<<< Updated upstream
=======
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
>>>>>>> Stashed changes
	if (AttackIdentifier >= 1)
	{
		switch (PatternIndex)
		{
		case 0: BulletSpawner->StartSequence(CircleCombo2); break;
		case 1: BulletSpawner->StartSequence(SphereCombo2); break;
		case 2: BulletSpawner->StartSequence(SpiralCombo2); break;
		case 3: BulletSpawner->StartSequence(SurroundCombo2); break;
		}
	}
	else
	{
		switch (PatternIndex)
		{
		case 0: BulletSpawner->StartSequence(CircleCombo); break;
		case 1: BulletSpawner->StartSequence(SphereCombo); break;
		case 2: BulletSpawner->StartSequence(SpiralCombo); break;
		case 3: BulletSpawner->StartSequence(SurroundCombo); break;
		}
	}
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

void ASerXBoss::MoveTo(FVector Target)
{
	if (bIsDead) return;

	MovementTarget = Target;
	bIsMoving = true;

	if (bIsRecording)
	{
		UMoveCommand* Cmd = NewObject<UMoveCommand>(this);
		Cmd->TargetLocation = Target;
		RecordCommand(Cmd);
	}
}

void ASerXBoss::OnLinkerSpawnTimer()
{
	DoSpawnLinker();
}

void ASerXBoss::OnZoneSpawnTimer()
{
	ActivateZone();

	if (!bIsRecording && !bIsClone)
	{
		if (bRecordingStarted && AttackIdentifier < 1) return;
		StartRecording();
	}
}

void ASerXBoss::OnRecordingFinished()
{
	bIsRecording = false;
	StopRecordingAndSpawnClone();
}

void ASerXBoss::StartRecording()
{
	if (bIsRecording || bIsClone) return;

	bIsRecording = true;
	bRecordingStarted = true;
	RecordedCommands.Empty();
	RecordingStartTime = GetWorld()->GetTimeSeconds();
	UE_LOG(LogTemp, Log, TEXT("SerXBoss started recording commands."));

	GetWorldTimerManager().SetTimer(RecordingTimerHandle, this, &ASerXBoss::OnRecordingFinished, RecordingDuration, false);
}

void ASerXBoss::StopRecordingAndSpawnClone()
{
	if (RecordedCommands.Num() == 0 || !GetWorld()) return;

	FTransform SpawnTransform(FRotator::ZeroRotator, GetActorLocation() + FVector(800.f, 0.f, 0.f));

	ASerXBoss* Clone = GetWorld()->SpawnActorDeferred<ASerXBoss>(GetClass(), SpawnTransform);
	if (Clone)
	{
		Clone->bIsClone = true;
		Clone->AttackInterval = 2.5f;

		float CloneHealth = FMath::Max(1.f, HealthComp->CurrentHealth * 0.1f);
		Clone->HealthComp->MaxHealth = CloneHealth;
		Clone->HealthComp->CurrentHealth = CloneHealth;

		Clone->FinishSpawning(SpawnTransform);

		PlaybackCommands(Clone);
	}
}

void ASerXBoss::PlaybackCommands(ASerXBoss* Clone)
{
	if (!Clone) return;

	TArray<UBossCommand*> CommandsCopy = RecordedCommands;
	TWeakObjectPtr<ASerXBoss> WeakClone = Clone;

	for (int32 i = 0; i < CommandsCopy.Num(); ++i)
	{
		if (!CommandsCopy[i]) continue;

		float Delay = CommandsCopy[i]->Timestamp;

		FTimerHandle TimerHandle;
		FTimerDelegate Delegate;
		Delegate.BindLambda([CommandsCopy, i, WeakClone]()
		{
			if (ASerXBoss* ValidClone = WeakClone.Get())
			{
				if (CommandsCopy.IsValidIndex(i) && CommandsCopy[i])
				{
					CommandsCopy[i]->Execute(ValidClone);
				}
			}
		});
		GetWorldTimerManager().SetTimer(TimerHandle, Delegate, Delay, false);
	}
	float TotalDuration = CommandsCopy.Num() > 0 ? CommandsCopy.Last()->Timestamp : RecordingDuration;

	FTimerHandle DestroyTimerHandle;
	FTimerDelegate DestroyDelegate;
	DestroyDelegate.BindLambda([WeakClone]()
	{
		if (ASerXBoss* ValidClone = WeakClone.Get())
		{
			ValidClone->bIsDead = true;
			ValidClone->StopAttackLoop();
			ValidClone->Destroy();
		}
	});
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, DestroyDelegate, TotalDuration + 2.0f, false);
}

void ASerXBoss::RecordCommand(UBossCommand* Cmd)
{
	if (Cmd)
	{
		Cmd->Timestamp = RecordingStartTime >= 0.f
			? GetWorld()->GetTimeSeconds() - RecordingStartTime
			: 0.f;
		RecordedCommands.Add(Cmd);
		UE_LOG(LogTemp, Log, TEXT("SerXBoss recorded command: %s at time %.2f"), *Cmd->GetClass()->GetName(), Cmd->Timestamp);
	}
}

void ASerXBoss::SetLinkerFactory(UChronostasisFactoryEnemy* Factory)
{
	LinkerFactory = Factory;
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
	return ABossBase::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ASerXBoss::Die()
{
	if (bIsDead) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LinkerSpawnTimerHandle);
		World->GetTimerManager().ClearTimer(ZoneSpawnTimerHandle);
		World->GetTimerManager().ClearTimer(RecordingTimerHandle);
	}

	Super::Die();
}
