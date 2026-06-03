// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/OrchestratorStates.h"
#include "Enemies/Orchestrator/Orchestrator.h"
#include "Components/RhytmConductorComponent.h"
#include "Components/BulletSpawnerComponent.h"

FVector GetScaleFromBPM(float CurrentBPM)
{
	FVector2D BPMRange(60.0f, 180.0f);     // De lento a rápido
	FVector2D ScaleRange(2.6f, 1.0f);      // De gigante a pequeño (relación inversa)

	// Interpola el valor de forma segura
	float UniformScale = FMath::GetMappedRangeValueClamped(BPMRange, ScaleRange, CurrentBPM);

	return FVector(UniformScale, UniformScale, UniformScale);
}

void UOrchestrator_Normal::EnterState(ABossBase* Boss)
{
	OrchestratorRef = Cast<AOrchestrator>(Boss);
	OrchestratorRef->HealthComp->SetInvulnerable(false);
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Nos suscribimos al evento del metrónomo
		OrchestratorRef->RhythmConductor->OnBeat.AddDynamic(this, &UOrchestrator_Normal::HandleBeat);

		// Iniciamos ritmo 4/4 clásico de la Fase 1
		OrchestratorRef->RhythmConductor->StartRhythm(120.0f);
	}
}

void UOrchestrator_Normal::ExitState(ABossBase* Boss)
{
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Siempre debemos desuscribirnos al cambiar de estado para evitar memory leaks
		OrchestratorRef->RhythmConductor->OnBeat.RemoveDynamic(this, &UOrchestrator_Normal::HandleBeat);
	}
}

void UOrchestrator_Normal::HandleBeat()
{
	if (!OrchestratorRef || !OrchestratorRef->BulletSpawner) return;

	BeatCounter++;
	FVector DynamicScale = GetScaleFromBPM(120.0f);
	TArray<FAttackStep> CurrentBeatAttack;

	// Cada 16 pulsos (4 compases completos), interrumpimos el patrón normal y lanzamos el gran ataque
	if (BeatCounter % 16 == 0)
	{
		// Lanzamos el Pentagrama. Le ponemos un Delay ligeramente mayor para darle "peso" al ataque.
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Pentagram, 100, 500.0f, 0.5f, 0.0f, 10.0f, 0.05f, DynamicScale));
	}
	else
	{
		// Patrón clásico 4/4
		int32 Compas = BeatCounter % 4;

		if (Compas == 1) // Fuerte
			CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 12, 600.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
		else if (Compas == 2) // Débil (silencio estratégico)
		{
		}
		else if (Compas == 3) // Medio
			CurrentBeatAttack.Add(FAttackStep(EAttackType::Burst, 3, 800.0f, 0.0f, 0.0f, 10.0f, 0.2f, DynamicScale));
		else if (Compas == 0) // Cierre
			CurrentBeatAttack.Add(FAttackStep(EAttackType::Spiral, 20, 500.0f, 0.0f, 15.0f, 10.0f, 0.1f, DynamicScale));
	}

	if (CurrentBeatAttack.Num() > 0) OrchestratorRef->BulletSpawner->StartSequence(CurrentBeatAttack);
}

//----------------------------------------------------------------------
void UOrchestrator_Melancholy::EnterState(ABossBase* Boss)
{
	OrchestratorRef = Cast<AOrchestrator>(Boss);
	OrchestratorRef->HealthComp->SetInvulnerable(false);
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Nos suscribimos al evento del metrónomo
		OrchestratorRef->RhythmConductor->OnBeat.AddDynamic(this, &UOrchestrator_Melancholy::HandleBeat);

		// Iniciamos ritmo 4/4 clásico de la Fase 1
		OrchestratorRef->RhythmConductor->StartRhythm(60.0f);
	}
}

void UOrchestrator_Melancholy::ExitState(ABossBase* Boss)
{
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Siempre debemos desuscribirnos al cambiar de estado para evitar memory leaks
		OrchestratorRef->RhythmConductor->OnBeat.RemoveDynamic(this, &UOrchestrator_Melancholy::HandleBeat);
		OrchestratorRef->HealthComp->SetInvulnerable(true);
	}
}

void UOrchestrator_Melancholy::HandleBeat()
{
	if (!OrchestratorRef || !OrchestratorRef->BulletSpawner) return;

	BeatCounter++;
	FVector DynamicScale = GetScaleFromBPM(60.0f); // Proyectiles GIGANTES
	TArray<FAttackStep> CurrentBeatAttack;

	int32 Compas = BeatCounter % 4;

	if (Compas == 1)
	{
		// Anillo muy lento y abrumador
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 16, 300.0f, 0.0f, 0.0f, 15.0f, 0.1f, DynamicScale));
	}
	else if (Compas == 3)
	{
		// Un anillo ligeramente más rápido para atrapar al jugador si se confía
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 16, 450.0f, 0.0f, 0.0f, 15.0f, 0.1f, DynamicScale));
	}

	if (CurrentBeatAttack.Num() > 0) OrchestratorRef->BulletSpawner->StartSequence(CurrentBeatAttack);
}

//----------------------------------------------------------------------

void UOrchestrator_Frenetic::EnterState(ABossBase* Boss)
{
	OrchestratorRef = Cast<AOrchestrator>(Boss);
	OrchestratorRef->HealthComp->SetInvulnerable(false);
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Nos suscribimos al evento del metrónomo
		OrchestratorRef->RhythmConductor->OnBeat.AddDynamic(this, &UOrchestrator_Frenetic::HandleBeat);

		// Iniciamos ritmo 4/4 clásico de la Fase 1
		OrchestratorRef->RhythmConductor->StartRhythm(160.0f);
	}
}

void UOrchestrator_Frenetic::ExitState(ABossBase* Boss)
{
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Siempre debemos desuscribirnos al cambiar de estado para evitar memory leaks
		OrchestratorRef->RhythmConductor->OnBeat.RemoveDynamic(this, &UOrchestrator_Frenetic::HandleBeat);
		OrchestratorRef->HealthComp->SetInvulnerable(true);
	}
}

void UOrchestrator_Frenetic::HandleBeat()
{
	if (!OrchestratorRef || !OrchestratorRef->BulletSpawner) return;

	BeatCounter++;
	FVector DynamicScale = GetScaleFromBPM(160.0f); // Proyectiles medianos/pequeños rápidos
	TArray<FAttackStep> CurrentBeatAttack;

	int32 Compas = BeatCounter % 4;

	// La percusión constante no da respiro
	if (Compas == 1 || Compas == 3)
	{
		// Ataques circulares masivos en los tiempos fuertes
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 24, 900.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
	}
	else
	{
		// Disparos directos en los contratiempos
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Burst, 2, 1000.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
	}

	// Adicional: Cada 8 pulsos (clímax menor), invoca una espiral sucia que se superpone a los círculos
	if (BeatCounter % 8 == 0)
	{
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Spiral, 24, 1000.0f, 0.0f, 25.0f, 10.0f, 0.1f, DynamicScale));
	}

	if (CurrentBeatAttack.Num() > 0) OrchestratorRef->BulletSpawner->StartSequence(CurrentBeatAttack);
}

//----------------------------------------------------------------------

void UOrchestrator_Furious::EnterState(ABossBase* Boss)
{
	OrchestratorRef = Cast<AOrchestrator>(Boss);
	OrchestratorRef->HealthComp->SetInvulnerable(false);
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Nos suscribimos al evento del metrónomo
		OrchestratorRef->RhythmConductor->OnBeat.AddDynamic(this, &UOrchestrator_Furious::HandleBeat);

		// Iniciamos ritmo 4/4 clásico de la Fase 1
		OrchestratorRef->RhythmConductor->StartRhythm(180.0f);
	}
}

void UOrchestrator_Furious::ExitState(ABossBase* Boss)
{
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Siempre debemos desuscribirnos al cambiar de estado para evitar memory leaks
		OrchestratorRef->RhythmConductor->OnBeat.RemoveDynamic(this, &UOrchestrator_Furious::HandleBeat);
	}
	OrchestratorRef->HealthComp->SetInvulnerable(true);
}

void UOrchestrator_Furious::HandleBeat()
{
	if (!OrchestratorRef || !OrchestratorRef->BulletSpawner) return;

	BeatCounter++;
	FVector DynamicScale = GetScaleFromBPM(180.0f); // Proyectiles diminutos y letales
	TArray<FAttackStep> CurrentBeatAttack;

	// El Pentagrama de la Fase 1 vuelve, pero el doble de rápido y mortal
	if (BeatCounter % 16 == 0)
	{
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Pentagram, 60, 1200.0f, 0.0f, 0.0f, 20.0f, 0.05f, DynamicScale));
	}

	int32 Compas = BeatCounter % 4;

	// Combinación de las fases anteriores por capas
	if (Compas == 1)
	{
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 16, 700.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Burst, 3, 1200.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
	}
	else if (Compas == 3)
	{
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Spiral, 15, 800.0f, 0.0f, 30.0f, 10.0f, 0.1f, DynamicScale));
	}

	if (CurrentBeatAttack.Num() > 0) OrchestratorRef->BulletSpawner->StartSequence(CurrentBeatAttack);
}

//----------------------------------------------------------------------

void UOrchestratorIntro::EnterState(ABossBase* Boss)
{
	UE_LOG(LogTemp, Warning, TEXT("Orchestrator acaba de entrar!"));
	AOrchestrator* Master = Cast<AOrchestrator>(Boss);
	Master->HealthComp->SetInvulnerable(true);

	Master->GetWorld()->GetTimerManager().SetTimer(Master->IntroTimer, [Master]()
		{
			if (Master)
			{
				Master->ChangeState(Master->Phase1State);
			}
		}, 3.0f, false);
}

void UOrchestratorIntro::ExitState(ABossBase* Boss)
{
	UE_LOG(LogTemp, Warning, TEXT("Orchestrator sale de estado Intro"));
}

void UOrchestratorIntro::HandleBeat()
{
}

//----------------------------------------------------------------------

void UOrchePhaseTransition::EnterState(ABossBase* Boss)
{
	AOrchestrator* Master = Cast<AOrchestrator>(Boss);
	if (!Master) return; // Protección por si acaso

	Master->HealthComp->SetInvulnerable(true);
	Master->RhythmConductor->StopRhythm();
	Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->AttackLoopTimer);

	if (!Boss->PhaseTransitionTimer.IsValid())
	{
		Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->PhaseTransitionTimer);
	}

	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->PhaseTransitionTimer, [Boss]()
		{
			AOrchestrator* Master = Cast<AOrchestrator>(Boss);
			if (Master && Master->HealthComp)
			{
				// 1. Quitamos la invulnerabilidad para que vuelva a recibir daño
				Master->HealthComp->SetInvulnerable(false);

				// 2. Calculamos la vida para saber a qué fase saltar
				float HealthPercent = Master->HealthComp->CurrentHealth / Master->HealthComp->MaxHealth;

				if (HealthPercent <= 0.75f && HealthPercent > 0.50f)
				{
					Boss->ChangeState(Master->Phase2State);
					UE_LOG(LogTemp, Warning, TEXT("Jefe cambio de fase II!"));
				}
				else if (HealthPercent <= 0.50f && HealthPercent > 0.25f)
				{
					Boss->ChangeState(Master->Phase3State);
					UE_LOG(LogTemp, Warning, TEXT("Jefe cambio de fase III!"));
				}
				else if (HealthPercent <= 0.25f)
				{
					Boss->ChangeState(Master->Phase4State);
					UE_LOG(LogTemp, Warning, TEXT("Jefe cambio de fase IV!"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("La vida no cuadra con ninguna fase."));
				}
			}
		}, 3.0f, false);
}

void UOrchePhaseTransition::ExitState(ABossBase* Boss)
{
}

void UOrchePhaseTransition::HandleBeat()
{
}