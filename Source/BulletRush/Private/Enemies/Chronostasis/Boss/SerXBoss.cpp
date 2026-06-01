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

	HealthComp->MaxHealth = 6000.f;

	AlteredZoneClass = AAlteredZone::StaticClass();

	MovementStrategy = CreateDefaultSubobject<USeekMovement>(TEXT("SeekMovement"));
	USeekMovement* Seek = Cast<USeekMovement>(MovementStrategy);
	if (Seek) Seek->Speed = 400.f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);
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

	SphereCombo2.Empty();
	SphereCombo2.Add(FAttackStep(EAttackType::Sphere, 80, 800.f, 1.6f, 0.f, 10.f));

	SpiralCombo2.Empty();
	SpiralCombo2.Add(FAttackStep(EAttackType::Spiral, 40, 500.f, 1.5f, 12.f, 15.f));

	SurroundCombo2.Empty();
	SurroundCombo2.Add(FAttackStep(EAttackType::SurroundingBullets, 40, 0.f, 1.8f, 0.f, 0.f));
}

void ASerXBoss::Attack()
{
	if (!BulletSpawner || bIsDead) return;

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
}

void ASerXBoss::ExecuteAttack(int32 PatternIndex)
{
	if (!BulletSpawner || bIsDead) return;

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

void ASerXBoss::DoSpawnCharger()
{
	if (!ChargerFactory || !GetWorld() || bIsDead) return;

	FVector SpawnLoc = GetActorLocation() + FMath::VRand() * 300.f;
	SpawnLoc.Z = GetActorLocation().Z;
	ChargerFactory->CreateEnemy(GetWorld(), SpawnLoc);
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

void ASerXBoss::SetChargerFactory(UChronostasisFactoryEnemy* Factory)
{
	ChargerFactory = Factory;
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
