#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "SerXBoss.generated.h"

class AAlteredZone;
class UBossCommand;
class UMovementStrat;

UCLASS(Blueprintable)
class BULLETRUSH_API ASerXBoss : public ABossBase
{
	GENERATED_BODY()

public:
	ASerXBoss();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Die() override;

	void SetChargerFactory(UChronostasisFactoryEnemy* Factory);
	void SetLinkerFactory(UChronostasisFactoryEnemy* Factory);

	void ExecuteAttack(int32 PatternIndex);
	void DoSpawnCharger();
	void DoSpawnLinker();
	void ActivateZone();
	void MoveTo(FVector Target);

	void StartRecording();
	void StopRecordingAndSpawnClone();
	void PlaybackCommands(ASerXBoss* Clone);

	bool bIsClone = false;

protected:
	virtual void BeginDestroy() override;

	virtual void Attack() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnLinkerSpawnTimer();
	UFUNCTION()
	void OnZoneSpawnTimer();
	UFUNCTION()
	void OnRecordingFinished();
	UFUNCTION()
	void OnLinkerDied(AEnemyBase* DeadLinker);

	void RecordCommand(UBossCommand* Cmd);
	void SetupAttackCombos();
	void SetupPhase2Combos();

	UPROPERTY()
	UMovementStrat* MovementStrategy;
	FVector MovementTarget;
	bool bIsMoving = false;

	UPROPERTY()
	TArray<UBossCommand*> RecordedCommands;
	float RecordingDuration = 10.f;
	float RecordingStartTime = -1.f;
	FTimerHandle RecordingTimerHandle;
	bool bIsRecording = false;
	bool bRecordingStarted = false;

	FTimerHandle LinkerSpawnTimerHandle;
	FTimerHandle ZoneSpawnTimerHandle;

	UPROPERTY()
	UChronostasisFactoryEnemy* ChargerFactory;
	UPROPERTY()
	UChronostasisFactoryEnemy* LinkerFactory;

	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<AAlteredZone> AlteredZoneClass;

	float ZoneSpawnInterval = 30.f;
	float LinkerSpawnInterval = 10.f;

	int32 AttackCount = 0;
	int32 AttacksSinceChargerSpawn = 0;
	int32 ActiveLinkerCount = 0;
	int32 MaxLinkers = 3;

	TArray<FAttackStep> CircleCombo;
	TArray<FAttackStep> SphereCombo;
	TArray<FAttackStep> SpiralCombo;
	TArray<FAttackStep> SurroundCombo;

	TArray<FAttackStep> CircleCombo2;
	TArray<FAttackStep> SphereCombo2;
	TArray<FAttackStep> SpiralCombo2;
	TArray<FAttackStep> SurroundCombo2;
};
