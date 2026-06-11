// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/OrchestratorStates.h"
#include "Animation/AnimSequence.h"
#include "Enemies/Orchestrator/Orchestrator.h"
#include "Components/RhytmConductorComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Core/BulletRushGameInstance.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

FVector GetScaleFromBPM(float CurrentBPM)
{
	FVector2D BPMRange(60.0f, 180.0f);     // De lento a rápido
	FVector2D ScaleRange(2.8f, 1.0f);      // De gigante a pequeño (relación inversa)

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

		if (OrchestratorRef->BossAudioComp && OrchestratorRef->Phase1Music)
		{
			OrchestratorRef->BossAudioComp->Stop();
			OrchestratorRef->BossAudioComp->SetSound(OrchestratorRef->Phase1Music);
			OrchestratorRef->BossAudioComp->FadeIn(2.0f);
			//OrchestratorRef->BossAudioComp->Play();
		}
		// Nos suscribimos al evento del metrónomo
		OrchestratorRef->RhythmConductor->OnBeat.AddDynamic(this, &UOrchestrator_Normal::HandleBeat);

		// Iniciamos ritmo 4/4 clásico de la Fase 1
		OrchestratorRef->RhythmConductor->StartRhythm(120.0f);
	}
	OrchestratorRef->GetWorld()->GetTimerManager().SetTimer(
		OrchestratorRef->RoamTimerHandle, OrchestratorRef, &AOrchestrator::RoamAroundPlayer, 4.0f, true);
}

void UOrchestrator_Normal::ExitState(ABossBase* Boss)
{
	if (OrchestratorRef)
	{
		if (OrchestratorRef->RhythmConductor)
		{
			// Siempre debemos desuscribirnos al cambiar de estado para evitar memory leaks
			OrchestratorRef->RhythmConductor->OnBeat.RemoveDynamic(this, &UOrchestrator_Normal::HandleBeat);
		}
	
		OrchestratorRef->GetWorld()->GetTimerManager().ClearTimer(OrchestratorRef->RoamTimerHandle);
	}
}

void UOrchestrator_Normal::HandleBeat()
{
	if (!OrchestratorRef || !OrchestratorRef->BulletSpawner) return;

	BeatCounter++;
	FVector DynamicScale = GetScaleFromBPM(120.0f);
	TArray<FAttackStep> CurrentBeatAttack;

	int32 Compas = BeatCounter % 4;

	if (UGameInstance* GameInst = OrchestratorRef->GetWorld()->GetGameInstance())
	{
		if (UProjectilesSubsystem* ProjSub = GameInst->GetSubsystem<UProjectilesSubsystem>())
		{
			// Si es el compás 1 (Fuerte), pasamos true. Si es otro, pasamos false.
			bool bIsStrongBeat = (Compas == 1);
			if (bIsStrongBeat)
				ProjSub->HandleBeatHit(bIsStrongBeat, 120.0f);
		}
	}

	// Cada 16 pulsos (4 compases completos), interrumpimos el patrón normal y lanzamos el gran ataque
	if (BeatCounter % 16 == 0)
	{
		// Lanzamos el Pentagrama. Le ponemos un Delay ligeramente mayor para darle "peso" al ataque.
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Pentagram, 100, 500.0f, 0.5f, 0.0f, 10.0f, 0.05f, DynamicScale));
	}
	else
	{

		if (Compas == 1) // Fuerte
			CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 12, 600.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
		else if (Compas == 2) // Débil (silencio estratégico)
		{
		}
		else if (Compas == 3) // Medio
			CurrentBeatAttack.Add(FAttackStep(EAttackType::Sphere, 24, 800.0f, 0.0f, 0.0f, 10.0f, 0.2f, DynamicScale));
		else if (Compas == 0) // Cierre
		{
			//CurrentBeatAttack.Add(FAttackStep(EAttackType::Spiral, 20, 500.0f, 0.0f, 15.0f, 10.0f, 0.1f, DynamicScale));
		}
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
		if (OrchestratorRef->BossAudioComp && OrchestratorRef->Phase2Music)
		{
			OrchestratorRef->BossAudioComp->Stop();
			OrchestratorRef->BossAudioComp->SetSound(OrchestratorRef->Phase2Music);
			OrchestratorRef->BossAudioComp->FadeIn(2.0f);
		}
		// Nos suscribimos al evento del metrónomo
		OrchestratorRef->RhythmConductor->OnBeat.AddDynamic(this, &UOrchestrator_Melancholy::HandleBeat);

		// Iniciamos ritmo 4/4 clásico de la Fase 1
		OrchestratorRef->RhythmConductor->StartRhythm(60.0f);
	}
	OrchestratorRef->GetWorld()->GetTimerManager().SetTimer(
		OrchestratorRef->RoamTimerHandle, OrchestratorRef, &AOrchestrator::RoamAroundPlayer, 4.0f, true);
}

void UOrchestrator_Melancholy::ExitState(ABossBase* Boss)
{
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		// Siempre debemos desuscribirnos al cambiar de estado para evitar memory leaks
		OrchestratorRef->RhythmConductor->OnBeat.RemoveDynamic(this, &UOrchestrator_Melancholy::HandleBeat);
		OrchestratorRef->HealthComp->SetInvulnerable(true);
		OrchestratorRef->GetWorld()->GetTimerManager().ClearTimer(OrchestratorRef->RoamTimerHandle);
	}
}

void UOrchestrator_Melancholy::HandleBeat()
{
	if (!OrchestratorRef || !OrchestratorRef->BulletSpawner) return;

	BeatCounter++;
	FVector DynamicScale = GetScaleFromBPM(60.0f); // Proyectiles GIGANTES
	TArray<FAttackStep> CurrentBeatAttack;

	int32 Compas = BeatCounter % 4;

	if (UGameInstance* GameInst = OrchestratorRef->GetWorld()->GetGameInstance())
	{
		if (UProjectilesSubsystem* ProjSub = GameInst->GetSubsystem<UProjectilesSubsystem>())
		{
			bool bIsStrongBeat = (Compas == 1);
			if (bIsStrongBeat)
				ProjSub->HandleBeatHit(bIsStrongBeat, 60.0f);
		}
	}

	if (Compas == 1)
	{
		// Anillo muy lento y abrumador
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 16, 300.0f, 0.0f, 0.0f, 15.0f, 0.1f, DynamicScale));
	}
	else if (Compas == 3)
	{
		// Un anillo ligeramente más rápido para atrapar al jugador si se confía
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Sphere, 16, 450.0f, 0.0f, 0.0f, 15.0f, 0.1f, DynamicScale));
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
		if (OrchestratorRef->BossAudioComp && OrchestratorRef->Phase3Music)
		{
			OrchestratorRef->BossAudioComp->Stop();
			OrchestratorRef->BossAudioComp->SetSound(OrchestratorRef->Phase3Music);
			OrchestratorRef->BossAudioComp->FadeIn(2.0f);
		}
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

	if (UGameInstance* GameInst = OrchestratorRef->GetWorld()->GetGameInstance())
	{
		if (UProjectilesSubsystem* ProjSub = GameInst->GetSubsystem<UProjectilesSubsystem>())
		{
			// Si es el compás 1 (Fuerte), pasamos true. Si es otro, pasamos false.
			if (Compas == 1 || Compas == 3 || BeatCounter % 8 == 0)
				ProjSub->HandleBeatHit(true, 160.0f);
		}
	}

	float HalfBeatDelay = (60.0f / 160.0f) / 2.0f;
	// Adicional: Cada 8 pulsos (clímax menor), invoca una espiral sucia que se superpone a los círculos
	if (BeatCounter % 8 == 0)
	{
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Spiral, 24, 1000.0f, 0.0f, 25.0f, 10.0f, 0.1f, DynamicScale));
	}
	if (BeatCounter % 16 == 0)
	{
		OrchestratorRef->ErraticTeleport();
	}
	if (Compas == 1)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Sphere, 24, 900.0f, HalfBeatDelay, 0.0f, 10.0f, 0.1f, DynamicScale));
	else if (Compas == 2)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::SurroundingBullets, 12, 1000.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
	else if (Compas == 3)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Sphere, 24, 900.0f, HalfBeatDelay, 0.0f, 10.0f, 0.1f, DynamicScale));
	else if (Compas == 4)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::SurroundingBullets, 12, 1000.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));

	if (CurrentBeatAttack.Num() > 0) OrchestratorRef->BulletSpawner->StartSequence(CurrentBeatAttack);
}

//----------------------------------------------------------------------

void UOrchestrator_Furious::EnterState(ABossBase* Boss)
{
	OrchestratorRef = Cast<AOrchestrator>(Boss);
	OrchestratorRef->HealthComp->SetInvulnerable(false);
	if (OrchestratorRef && OrchestratorRef->RhythmConductor)
	{
		if (OrchestratorRef->BossAudioComp && OrchestratorRef->Phase4Music)
		{
			OrchestratorRef->BossAudioComp->Stop();
			OrchestratorRef->BossAudioComp->SetSound(OrchestratorRef->Phase4Music);
			OrchestratorRef->BossAudioComp->FadeIn(2.0f);
		}
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
}

void UOrchestrator_Furious::HandleBeat()
{
	if (!OrchestratorRef || !OrchestratorRef->BulletSpawner) return;

	BeatCounter++;
	FVector DynamicScale = GetScaleFromBPM(180.0f); // Proyectiles diminutos y letales
	TArray<FAttackStep> CurrentBeatAttack;

	if (BeatCounter % 84 == 0)
	{
		if (UGameInstance* GameInst = OrchestratorRef->GetWorld()->GetGameInstance())
		{
			if (UProjectilesSubsystem* ProjSub = GameInst->GetSubsystem<UProjectilesSubsystem>())
			{
				ProjSub->HandleSilenceEnter();
				OrchestratorRef->RhythmConductor->TriggerSilence(true);

				FTimerHandle SilenceEndTimer;
				OrchestratorRef->GetWorld()->GetTimerManager().SetTimer(SilenceEndTimer, [this, ProjSub]()
					{
						if (OrchestratorRef && OrchestratorRef->RhythmConductor)
						{
							if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OrchestratorRef->GetWorld(), 0))
							{
								ProjSub->ExecuteSilenceCollapse(PlayerPawn->GetActorLocation(), 2500.0f);
							}

							// Retomamos el metrónomo
							OrchestratorRef->RhythmConductor->TriggerSilence(false);
						}
					}, 2.0f, false);
			}
		}
		return; // Salimos del HandleBeat
	}
	// El Pentagrama de la Fase 1 vuelve, pero el doble de rápido y mortal
	if (BeatCounter % 16 == 0)
	{
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Pentagram, 60, 1200.0f, 0.0f, 0.0f, 20.0f, 0.05f, DynamicScale));
	}

	if (BeatCounter % 8 == 0)
	{
		OrchestratorRef->ErraticTeleport();
	}

	int32 Compas = BeatCounter % 4;

	if (UGameInstance* GameInst = OrchestratorRef->GetWorld()->GetGameInstance())
	{
		if (UProjectilesSubsystem* ProjSub = GameInst->GetSubsystem<UProjectilesSubsystem>())
		{
			bool bIsStrongBeat = BeatCounter % 84 == 0;
			if (!bIsStrongBeat)
			{
				bIsStrongBeat = Compas == 1;
				ProjSub->HandleBeatHit(bIsStrongBeat, 180.0f);
			}
		}
	}

	// Combinación de las fases anteriores por capas
	if (Compas == 1)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Circle, 16, 700.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
	else if (Compas == 2)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Sphere, 3, 1200.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));
	else if (Compas == 3)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::Sphere, 15, 800.0f, 0.0f, 30.0f, 10.0f, 0.1f, DynamicScale));
	else if (Compas == 4)
		CurrentBeatAttack.Add(FAttackStep(EAttackType::SurroundingBullets, 12, 1000.0f, 0.0f, 0.0f, 10.0f, 0.1f, DynamicScale));

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
	if (Master && Master->BossAudioComp)
	{
		Master->BossAudioComp->FadeOut(0.5f, 0.0f);
	}
	if (UWorld* World = Boss->GetWorld())
	{
		// 1. Obtenemos el GameInstance correctamente
		if (UGameInstance* GameInst = World->GetGameInstance())
		{
			// 2. Extraemos el Subsistema desde el GameInstance
			if (UProjectilesSubsystem* ProjSub = GameInst->GetSubsystem<UProjectilesSubsystem>())
			{
				if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0))
				{
					// Redirigimos todas las balas vivas hacia la cabeza del jugador
					ProjSub->RedirectAllBossBulletsToTarget(PlayerPawn->GetActorLocation(), 1500.0f);
				}
			}
		}
	}


	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->PhaseTransitionTimer, [Boss]()
		{
			AOrchestrator* Master = Cast<AOrchestrator>(Boss);
			if (Master && Master->HealthComp)
			{
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
		}, 5.0f, false);
}

void UOrchePhaseTransition::ExitState(ABossBase* Boss)
{
}

void UOrchePhaseTransition::HandleBeat()
{
}

//--------------------------------------------------------------

void UOrcheDead::EnterState(ABossBase* Boss)
{
	OrchestratorRef = Cast<AOrchestrator>(Boss);
	AOrchestrator* Master = Cast<AOrchestrator>(Boss);

	if (OrchestratorRef->RhythmConductor) OrchestratorRef->RhythmConductor->StopRhythm();

	if (AAIController* AI = Cast<AAIController>(OrchestratorRef->GetController())) {
		float TimeToFall = 0.5f; // Valor por defecto seguro

		if (OrchestratorRef->DownLimits.Z != 0.0f)
		{
			OrchestratorRef->MovementComp->MaxSpeed = 3000.0f; // Velocidad de caída dramática
			FVector CurrentLoc = OrchestratorRef->GetActorLocation();
			FVector NewLocation = CurrentLoc;
			NewLocation.Z = OrchestratorRef->DownLimits.Z - 400.0f;

			// Ordenamos la caída
			AI->MoveToLocation(NewLocation, 10.0f, false, false);

			// --- CÁLCULO DEL IMPACTO ---
			float FallDistance = FMath::Abs(CurrentLoc.Z - NewLocation.Z);
			TimeToFall = FallDistance / 3000.0f; // Tiempo = Distancia / Velocidad
		};

		UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(OrchestratorRef->GetGameInstance());
		if (GI)
		{
			GI->MarcarMapaCompletado(FName("Map_05Boss"));
		}

		if (OrchestratorRef->BossAudioComp) {
			OrchestratorRef->BossAudioComp->FadeOut(2.5f, 0.0f);
			// Si declaras un OrchestratorRef->DeathSound en tu jefe, lo reproduces aquí:
			// OrchestratorRef->BossAudioComp->SetSound(OrchestratorRef->DeathSound);
			// OrchestratorRef->BossAudioComp->Play();
		}
		if (OrchestratorRef->DeathAnimation && OrchestratorRef->OrchestMesh)
		{
			OrchestratorRef->OrchestMesh->PlayAnimation(OrchestratorRef->DeathAnimation, false);
		}
		OrchestratorRef->GetWorld()->GetTimerManager().SetTimer(OrchestratorRef->DeathTimerHandle, [Master]()
			{
				if (Master) Master->Die();
			}, TimeToFall, false);
		
	}
}

	void UOrcheDead::ExitState(ABossBase * Boss) {};