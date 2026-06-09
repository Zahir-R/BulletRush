#include "Components/BossRecorderComponent.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"
#include "Combat/Commands/AttackCommand.h"
#include "Combat/Commands/MoveCommand.h"
#include "Engine/World.h"

UBossRecorderComponent::UBossRecorderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBossRecorderComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBossRecorderComponent::BeginDestroy()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RecordingTimerHandle);
	}
	Super::BeginDestroy();
}

void UBossRecorderComponent::TryStartRecording(int32 AttackIdentifier)
{
	if (bIsRecording) return;

	// Only record once per phase
	if (bRecordingStarted && AttackIdentifier < 1) return;

	ASerXBoss* OwnerBoss = Cast<ASerXBoss>(GetOwner());
	if (!OwnerBoss) return;

	// If currently in a movement step, defer until it completes
	if (OwnerBoss->IsExecutingMovementCombo())
	{
		bPendingRecordingOnMovement = true;
		return;
	}

	StartRecordingInternal();
}

void UBossRecorderComponent::StartRecordingInternal()
{
	bIsRecording = true;
	bRecordingStarted = true;
	RecordedCommands.Empty();
	RecordingStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f;

	GetWorld()->GetTimerManager().SetTimer(RecordingTimerHandle, this, &UBossRecorderComponent::OnRecordingFinished, RecordingDuration, false);
}

void UBossRecorderComponent::OnRecordingFinished()
{
	bIsRecording = false;
	StopRecordingAndSpawnClone();
}

void UBossRecorderComponent::OnMovementStepFinished()
{
	if (bPendingRecordingOnMovement)
	{
		bPendingRecordingOnMovement = false;
		StartRecordingInternal();
	}
}

void UBossRecorderComponent::RecordAttack(int32 PatternIndex)
{
	if (!bIsRecording) return;

	UAttackCommand* Cmd = NewObject<UAttackCommand>(GetOwner());
	Cmd->AttackPatternIndex = PatternIndex;
	RecordCommand(Cmd);
}

void UBossRecorderComponent::RecordMove(FVector Target, int32 StrategyIndex)
{
	if (!bIsRecording) return;

	UMoveCommand* Cmd = NewObject<UMoveCommand>(GetOwner());
	Cmd->TargetLocation = Target;
	Cmd->StrategyIndex = StrategyIndex;
	RecordCommand(Cmd);
}

void UBossRecorderComponent::RecordCommand(UBossCommand* Cmd)
{
	if (Cmd)
	{
		Cmd->Timestamp = RecordingStartTime >= 0.f && GetWorld()
			? GetWorld()->GetTimeSeconds() - RecordingStartTime
			: 0.f;
		RecordedCommands.Add(Cmd);
	}
}

void UBossRecorderComponent::StopRecordingAndSpawnClone()
{
	if (RecordedCommands.Num() == 0 || !GetWorld()) return;

	ASerXBoss* OwnerBoss = Cast<ASerXBoss>(GetOwner());
	if (!OwnerBoss) return;

	FTransform SpawnTransform(FRotator::ZeroRotator, OwnerBoss->GetActorLocation() + FVector(800.f, 0.f, 0.f));

	ASerXBoss* Clone = GetWorld()->SpawnActorDeferred<ASerXBoss>(OwnerBoss->GetClass(), SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Clone)
	{
		Clone->bIsClone = true;
		Clone->AttackInterval = 2.5f;

		float CloneHealth = FMath::Max(1.f, OwnerBoss->HealthComp->CurrentHealth * 0.1f);
		Clone->HealthComp->MaxHealth = CloneHealth;
		Clone->HealthComp->CurrentHealth = CloneHealth;

		Clone->FinishSpawning(SpawnTransform);

		PlaybackCommands(Clone);
	}
}

void UBossRecorderComponent::PlaybackCommands(ASerXBoss* Clone)
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
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, Delay, false);
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
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, DestroyDelegate, TotalDuration + 2.0f, false);
}
