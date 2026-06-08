// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "Components/AudioComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Orchestrator.generated.h"

class URhytmConductorComponent;
class UOrchestrator_Normal;
class UOrchestrator_Melancholy;
class UOrchestrator_Frenetic;
class UOrchestrator_Furious;
class UOrchestratorIntro;
class UOrchePhaseTransition;
class UOrcheDead;

UCLASS()
class BULLETRUSH_API AOrchestrator : public ABossBase
{
	GENERATED_BODY()
	
public:
	AOrchestrator();

	virtual FString GetBossDisplayName() const override { return TEXT("ORCHESTRATOR"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	class UCapsuleComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	class USkeletalMeshComponent* OrchestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fisicas")
	class UPhysicsAsset* PhysicsAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* BossAudioComp;
	UPROPERTY(EditDefaultsOnly, Category = "Music")
	USoundBase* Phase1Music;

	UPROPERTY(EditDefaultsOnly, Category = "Music")
	USoundBase* Phase2Music;

	UPROPERTY(EditDefaultsOnly, Category = "Music")
	USoundBase* Phase3Music;

	UPROPERTY(EditDefaultsOnly, Category = "Music")
	USoundBase* Phase4Music;

	FTimerHandle DeathTimerHandle;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
							class AController* EventInstigator, AActor* DamageCauser) override;
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URhytmConductorComponent* RhythmConductor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Secret Level")
	bool bSecretLevelCleared;

	UPROPERTY()
	UOrchestrator_Normal* Phase1State;

	UPROPERTY()
	UOrchestrator_Melancholy* Phase2State;

	UPROPERTY()
	UOrchestrator_Frenetic* Phase3State;

	UPROPERTY()
	UOrchestrator_Furious* Phase4State;

	UPROPERTY()
	UOrchestratorIntro* IntroOrcheState;

	UPROPERTY()
	UOrchePhaseTransition* PhaseTransitionOrcheState;

	UPROPERTY()
	UOrcheDead* OrcheDeadState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	UFloatingPawnMovement* MovementComp;

	// Temporizador para el movimiento de la Fase 1 y 2
	FTimerHandle RoamTimerHandle;

	UFUNCTION()
	void RoamAroundPlayer();

	UFUNCTION()
	void ErraticTeleport();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Die() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mundo")
	FVector DownLimits = FVector(2300.0f, -6710.0f, 500.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mundo")
	FVector UpperLimits = FVector(10820.0f, 2080.0f, 2000.0f);

};
