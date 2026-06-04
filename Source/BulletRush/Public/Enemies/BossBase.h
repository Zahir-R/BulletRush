// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemies/EnemyBase.h"
#include "Components/HealthComponent.h"
#include "Components/WeakPointComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BossBase.generated.h"

class UShapeComponent;
class UBulletSpawnerComponent;
class UWeakPointComponent;
class UBossState;
class UBossStateIntro;
class UBossStateIdle;
class UBossStateAttacking;
class UBossStateStunned;
class UBossStatePhaseTransition;
class UBossStateDead;

class APortalManager;
UCLASS(Blueprintable)
class BULLETRUSH_API ABossBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABossBase();

protected:
	virtual void BeginPlay() override;

	int32 ActiveWeakPoints;

	UFUNCTION()
	virtual void HandleWeakPointDestroyed();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void ChangeState(UBossState* NewState);

	UFUNCTION()
	void OnTestWeakDestroyed();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	FName GetCurrentBossStateName() const;

	virtual void Attack() override;
	void ScheduleNextAttack();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BossMesh;

	FTimerHandle IntroTimer;
	FTimerHandle StunnedTimer;
	FTimerHandle PhaseTransitionTimer;

	void SetInvulnerable(bool newstate);

	int32 AttackIdentifier = 0;

	UBulletSpawnerComponent* BulletSpawner;
	FTimerHandle AttackLoopTimer;

	//UWeakPointComponent* TestWeak;

	TArray<FAttackStep> Combo;
	TArray<FAttackStep> Combo2;

	bool bHasTransitioned = false;

	UPROPERTY()
	UBossState* CurrentStateObject;

	UPROPERTY()
	UBossStateIntro* IntroState;

	UPROPERTY()
	UBossStateIdle* IdleState;

	UPROPERTY()
	UBossStateAttacking* AttackingState;

	UPROPERTY()
	UBossStateStunned* StunnedState;

	UPROPERTY()
	UBossStatePhaseTransition* PhaseTransitionState;

	UPROPERTY()
	UBossStateDead* DeadState;

	void Die() override;
	APortalManager* PortalManagerRef;
	bool HasActiveWeakPoints() const { return ActiveWeakPoints > 0; }
};
