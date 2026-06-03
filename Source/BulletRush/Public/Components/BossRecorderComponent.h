#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossRecorderComponent.generated.h"

class ASerXBoss;
class UBossCommand;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BULLETRUSH_API UBossRecorderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossRecorderComponent();

	void TryStartRecording(int32 AttackIdentifier);
	void StopRecordingAndSpawnClone();
	void OnMovementStepFinished();
	void RecordAttack(int32 PatternIndex);
	void RecordMove(FVector Target, int32 StrategyIndex);

	bool IsRecording() const { return bIsRecording; }
	bool HasPendingRecording() const { return bPendingRecordingOnMovement; }
	bool HasRecordedOnce() const { return bRecordingStarted; }

	void PlaybackCommands(ASerXBoss* Clone);

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	void StartRecordingInternal();

	UFUNCTION()
	void OnRecordingFinished();

	void RecordCommand(UBossCommand* Cmd);

	UPROPERTY()
	TArray<UBossCommand*> RecordedCommands;

	float RecordingDuration = 10.f;
	float RecordingStartTime = -1.f;
	FTimerHandle RecordingTimerHandle;

	bool bIsRecording = false;
	bool bRecordingStarted = false;
	bool bPendingRecordingOnMovement = false;
};
