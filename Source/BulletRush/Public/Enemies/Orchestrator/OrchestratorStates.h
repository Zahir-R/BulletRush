// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/State/BossStateBase.h"
#include "Enemies/State/BossStateIntro.h"
#include "Enemies/State/BossStatePhaseTransition.h"
#include "Enemies/State/BossStateDead.h"
#include "OrchestratorStates.generated.h"

class AOrchestrator;

UCLASS()
class BULLETRUSH_API UOrchestrator_Normal : public UBossState
{
	GENERATED_BODY()
public:
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override { return "Phase1_Normal"; }

	// Debe ser UFUNCTION para poder enlazarse al delegado
	UFUNCTION()
	void HandleBeat();

	int32 BeatCounter = 0;
private:
	UPROPERTY()
	AOrchestrator* OrchestratorRef;
	
};

UCLASS()
class BULLETRUSH_API UOrchestrator_Melancholy : public UBossState
{
	GENERATED_BODY()
public:
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override { return "Phase2_Melancholy"; }

	// Debe ser UFUNCTION para poder enlazarse al delegado
	UFUNCTION()
	void HandleBeat();

private:
	UPROPERTY()
	AOrchestrator* OrchestratorRef;

	int32 BeatCounter = 0;
};

UCLASS()
class BULLETRUSH_API UOrchestrator_Frenetic : public UBossState
{
	GENERATED_BODY()
public:
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override { return "Phase3_Frenetic"; }

	// Debe ser UFUNCTION para poder enlazarse al delegado
	UFUNCTION()
	void HandleBeat();

private:
	UPROPERTY()
	AOrchestrator* OrchestratorRef;

	int32 BeatCounter = 0;

};

UCLASS()
class BULLETRUSH_API UOrchestrator_Furious : public UBossState
{
	GENERATED_BODY()
public:
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override { return "Phase4_Furious"; }

	// Debe ser UFUNCTION para poder enlazarse al delegado
	UFUNCTION()
	void HandleBeat();

private:
	UPROPERTY()
	AOrchestrator* OrchestratorRef;

	int32 BeatCounter = 0;

};

UCLASS()
class BULLETRUSH_API UOrchestratorIntro : public UBossStateIntro
{
	GENERATED_BODY()
public:
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override { return "Intro"; }

	// Debe ser UFUNCTION para poder enlazarse al delegado
	UFUNCTION()
	void HandleBeat();

private:
	UPROPERTY()
	AOrchestrator* OrchestratorRef;

};

UCLASS()
class BULLETRUSH_API UOrchePhaseTransition : public UBossStatePhaseTransition
{
	GENERATED_BODY()
public:
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override { return "Transition"; }

	// Debe ser UFUNCTION para poder enlazarse al delegado
	UFUNCTION()
	void HandleBeat();

private:
	UPROPERTY()
	AOrchestrator* OrchestratorRef;

};

UCLASS()
class BULLETRUSH_API UOrcheDead : public UBossStateDead
{
	GENERATED_BODY()
public:
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override { return "OrcheDead"; }

private:
	UPROPERTY()
	AOrchestrator* OrchestratorRef;

};