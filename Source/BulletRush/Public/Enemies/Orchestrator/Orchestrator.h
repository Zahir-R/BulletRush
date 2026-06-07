// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "Components/AudioComponent.h"
#include "Orchestrator.generated.h"

class URhytmConductorComponent;
class UOrchestrator_Normal;
class UOrchestrator_Melancholy;
class UOrchestrator_Frenetic;
class UOrchestrator_Furious;
class UOrchestratorIntro;
class UOrchePhaseTransition;

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
};
