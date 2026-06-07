#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "SerXBoss.generated.h"

class AAlteredZone;
class UMovementStrat;
class USeekMovement;
class UMoveBehindMovement;
class UTriangulationMovement;
class UAscendMovement;
class UDescendMovement;
class UStaticMovement;
class UBossRecorderComponent;

struct FMovementComboStep
{
	int32 StrategyIndex = 0;
	float Duration = 0.f; // 0 = wait for strategy->bCompleted
};

UCLASS(Blueprintable)
class BULLETRUSH_API ASerXBoss : public ABossBase
{
	GENERATED_BODY()

public:
	ASerXBoss();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Die() override;

	void SetLinkerFactory(UChronostasisFactoryEnemy* Factory);
	void AddMinionFactory(UChronostasisFactoryEnemy* Factory);

	void ExecuteAttack(int32 PatternIndex);
	void ExecuteMovement(int32 StrategyIndex, FVector Target);
	void DoSpawnLinker();
	void ActivateZone();
	void MoveTo(FVector Target, int32 StrategyIndex = 0);
	void SpawnRandomMinion();

	bool IsExecutingMovementCombo() const { return bExecutingMovementCombo; }

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
	void OnLinkerDied(AEnemyBase* DeadLinker);

	void SetupAttackCombos();
	void SetupMovementStrategies();

	void StartAttackCombo(int32 ComboIndex);
	void AdvanceAttackCombo();
	void StartMovementCombo(int32 ComboIndex);
	void AdvanceMovementCombo();
	void StartMovementStep(int32 StepIndex);
	void FireAttackStep(const FAttackStep& Step);
	FVector GetMovementTargetForStrategy(int32 StrategyIndex);

	UPROPERTY()
	UBossRecorderComponent* RecorderComponent;

	UPROPERTY()
	UMovementStrat* MovementStrategy;
	UPROPERTY()
	USeekMovement* SeekStrat;
	UPROPERTY()
	UMoveBehindMovement* MoveBehindStrat;
	UPROPERTY()
	UTriangulationMovement* TriangulationStrat;
	UPROPERTY()
	UAscendMovement* AscendStrat;
	UPROPERTY()
	UDescendMovement* DescendStrat;
	UPROPERTY()
	UStaticMovement* StaticStrat;
	FVector MovementTarget;
	bool bIsMoving = false;
	int32 CurrentStrategyIndex = 0;

	int32 CurrentAttackCombo = -1;
	int32 CurrentAttackStep = 0;
	float AttackComboElapsed = 0.f;
	bool bExecutingAttackCombo = false;

	TArray<TArray<FMovementComboStep>> MoveCombos;
	int32 CurrentMovementCombo = -1;
	int32 CurrentMovementStep = 0;
	float MovementStepElapsed = 0.f;
	bool bExecutingMovementCombo = false;

	FTimerHandle LinkerSpawnTimerHandle;
	FTimerHandle ZoneSpawnTimerHandle;

	UPROPERTY()
	UChronostasisFactoryEnemy* LinkerFactory;
	UPROPERTY()
	TArray<UChronostasisFactoryEnemy*> MinionFactories;

	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<AAlteredZone> AlteredZoneClass;

	float ZoneSpawnInterval = 30.f;
	float LinkerSpawnInterval = 10.f;

	int32 StepsSinceMinionSpawn = 0;
	int32 ActiveLinkerCount = 0;
	int32 MaxLinkers = 3;

	TArray<TArray<FAttackStep>> AttackCombos;
	float RotationSpeed = 5.0f;
};
