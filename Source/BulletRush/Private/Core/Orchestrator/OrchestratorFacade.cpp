// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Core/BulletRushGameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/Orchestrator/Orchestrator.h"
#include "Enemies/EnemyBase.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Core/BulletRushHUD.h"
#include "Enemies/Orchestrator/BossArenaTrigger.h"
#include "Enemies/Bloodseeker/KamikazeEnemy.h"
#include "Enemies/Orchestrator/SecretGuardian.h"
#include "Player/PlayingPlayer.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AOrchestratorFacade::AOrchestratorFacade()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsPlayerDetected = false;
	bSecretPuzzleSolved = false;
	bBossFightStarted = false;

	GeneratorsDestroyed = 0;
	GuardiansDefeated = 0;
	TotalGuardiansToSpawn = 3;
	ReinforcementClass = AKamikazeEnemy::StaticClass();
	SecretGuardianClass = ASecretGuardian::StaticClass();

	LevelAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("LevelAudioComp"));
	RootComponent = LevelAudioComp;

	LevelAudioComp->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> AudioChill(TEXT("SoundWave'/Game/ParagonMuriel/OrchestratorMusic/Wav/Orchestrator_Idle.Orchestrator_Idle'"));
	if (AudioChill.Succeeded())
	{
		ChillMusic = AudioChill.Object;
	}
}

void AOrchestratorFacade::BeginPlay()
{
	Super::BeginPlay();

	if (LevelAudioComp && ChillMusic)
	{
		LevelAudioComp->SetSound(ChillMusic);
		LevelAudioComp->FadeIn(2.0f);
	}
	if (GetWorld())
	{
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABossArenaTrigger::StaticClass());

		if (FoundActor)
		{
			// Hacemos un Cast seguro para guardarlo en nuestra variable
			TriggerRef = Cast<ABossArenaTrigger>(FoundActor);
			UE_LOG(LogTemp, Warning, TEXT("Fachada: Referencia al Trigger del Jefe encontrada con éxito."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Fachada: ERROR. No se encontró ningún Trigger del Jefe en el nivel."));
		}
	}
	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (GI)
	{
		if (GI->OrchestratorLState == ELevelState::Boss)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fachada: El nivel se ha cargado en estado BOSS. Preparando arena..."));
			if (TriggerRef)
			{
				TriggerRef->PrepareArena();
			}
		}
		else 
		{
			GI->OrchestratorLState = ELevelState::Normal;
		}
	}
}

void AOrchestratorFacade::HandlePlayerDetected(FVector DetectionLocation)
{
	if (bIsPlayerDetected || bBossFightStarted) return; // Evitar spam

	bIsPlayerDetected = true;
	UE_LOG(LogTemp, Warning, TEXT("Fachada: ¡Jugador detectado! Sigilo roto. Invocando refuerzos..."));

	// Generamos los refuerzos que dicta el GDD alrededor del punto de detección
	SpawnZoneAReinforcements(DetectionLocation);
}

void AOrchestratorFacade::ReportGeneratorDestroyed()
{
	GeneratorsDestroyed++;
	UE_LOG(LogTemp, Warning, TEXT("Fachada: Generador destruido (%d/3)."), GeneratorsDestroyed);

	if (GeneratorsDestroyed == 1)
	{
		// Inicia el cronómetro de 5 segundos para el puzzle
		PuzzleTimeRemaining = 5.0f;
		bPuzzleActive = true;
		GetWorld()->GetTimerManager().SetTimer(PuzzleTimerHandle, this, &AOrchestratorFacade::FailSecretPuzzle, 5.0f, false);
	}
	else if (GeneratorsDestroyed >= 3)
	{
		bPuzzleActive = false;
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ABulletRushHUD* HUD = Cast<ABulletRushHUD>(PC->GetHUD())) HUD->SetCountdown(-1.0f);
		}
		// Puzzle resuelto a tiempo
		GetWorld()->GetTimerManager().ClearTimer(PuzzleTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Fachada: Puzzle a tiempo. Spawneando Guardianes Secretos."));
		if (GetWorld())
		{
			TArray<AActor*> SecretWalls;

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SecretWall"), SecretWalls);

			for (AActor* Wall : SecretWalls)
			{
				if (Wall)
				{
					// Los hacemos visibles
					Wall->SetActorHiddenInGame(false);
					// Les devolvemos la colisión para encerrar al jugador
					Wall->SetActorEnableCollision(true);

					UStaticMeshComponent* MeshComp = Wall->FindComponentByClass<UStaticMeshComponent>();
					if (MeshComp)
					{
						// Le asignamos el perfil estándar de los muros inamovibles
						MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
					}
				}
			}
			TArray<AActor*> Nivel1;

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("OrcheNivel1"), Nivel1);

			for (AActor* ToDelete : Nivel1)
			{
				if (ToDelete)
				{
					ToDelete->Destroy();
				}
			}
		}
		UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
		if (GI)
		{
			GI->OrchestratorLState = ELevelState::Secret;
		}
		SpawnSecretGuardians();
	}
}

void AOrchestratorFacade::FailSecretPuzzle()
{
	GeneratorsDestroyed = 0;
	bPuzzleActive = false;

	// Ocultamos el cronómetro al fallar
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ABulletRushHUD* HUD = Cast<ABulletRushHUD>(PC->GetHUD())) HUD->SetCountdown(-1.0f);
	}
	if (TriggerRef)
	{
		TriggerRef->PrepareArena();
	}
	// Aquí podrías destruir los generadores restantes o bloquear la habitación
}

void AOrchestratorFacade::ReportGuardianDefeated()
{
	GuardiansDefeated++;

	if (GuardiansDefeated >= TotalGuardiansToSpawn)
	{
		bSecretPuzzleSolved = true;
		UE_LOG(LogTemp, Warning, TEXT("Fachada: ¡Guardianes derrotados! Nivel 5-1-S completado. Jefe perderá Fase 1."));
		if (TriggerRef)
		{
			TriggerRef->PrepareArena();
		}
	}
}

void AOrchestratorFacade::PrepareBossArena(FTransform BossSpawnTransform)
{
	// 1. Evitamos dobles ejecuciones
	if (bBossFightStarted) return;
	bBossFightStarted = true;

	UE_LOG(LogTemp, Warning, TEXT("Fachada: Combate de Jefe Registrado."));

	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->OrchestratorLState = ELevelState::Boss;
		bBossFightStarted = false;
	}
	if (GetWorld())
	{
		AOrchestrator* FinalBoss = GetWorld()->SpawnActor<AOrchestrator>(AOrchestrator::StaticClass(), BossSpawnTransform);
		FinalBoss->SetActorRelativeLocation(FVector::ZeroVector);
		FinalBoss->SetActorLocation(BossSpawnTransform.GetLocation());

		TArray<AActor*> BossWalls;
		// Buscamos todos los actores del nivel que tengan la etiqueta "BossWall"
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BossWall"), BossWalls);

		for (AActor* Wall : BossWalls)
		{
			if (Wall)
			{
				// Los hacemos visibles
				Wall->SetActorHiddenInGame(false);
				// Les devolvemos la colisión para encerrar al jugador
				Wall->SetActorEnableCollision(true);

				UStaticMeshComponent* MeshComp = Wall->FindComponentByClass<UStaticMeshComponent>();
				if (MeshComp)
				{
					// Le asignamos el perfil estándar de los muros inamovibles
					MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
				}
			}

			TArray<AActor*> Nivel1;
			TArray<AActor*> NivelS;

			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("OrcheNivel1"), Nivel1);
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("OrcheNivelS"), NivelS);

			for (AActor* ToDelete : Nivel1)
			{
				if (ToDelete)
				{
					ToDelete->Destroy();
				}
			}

			for (AActor* ToDelete : NivelS)
			{
				if (ToDelete)
				{
					ToDelete->Destroy();
				}
			}
		}
	}
	if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GM->LevelFacade && GM->LevelFacade->LevelAudioComp)
		{
			// Apaga la música de exploración/combate en 2 segundos
			GM->LevelFacade->LevelAudioComp->FadeOut(2.0f, 0.0f);
		}
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APlayingPlayer* PlayingPlayer = Cast<APlayingPlayer>(PC->GetPawn());
	PlayingPlayer->SetActorLocation(FVector(4410.0f, -2710.0f, 50.0f));
}

void AOrchestratorFacade::SpawnZoneAReinforcements(FVector SpawnOrigin)
{
	if (!ReinforcementClass || !GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn de 3 enemigos en un triángulo alrededor del jugador
	for (int i = 0; i < 3; i++)
	{
		FVector Offset = FVector(FMath::Cos(i * 120.0f) * 400.0f, FMath::Sin(i * 120.0f) * 400.0f, 650.0f);
		GetWorld()->SpawnActor<AEnemyBase>(ReinforcementClass, SpawnOrigin + Offset, FRotator::ZeroRotator, SpawnParams);
	}
}

void AOrchestratorFacade::SpawnSecretGuardians()
{
	if (!SecretGuardianClass || !GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int i = 0; i < TotalGuardiansToSpawn; i++)
	{
		// Distribuimos a los guardianes en una línea horizontal (Y) separada por 500 unidades
		FVector Offset = FVector(0.0f, (i - 1) * 500.0f, 0.0f);
		FVector SpawnLoc = FVector(0.0f, -4730.0f, 500.0f) + Offset;

		AEnemyBase* Guardian = GetWorld()->SpawnActor<AEnemyBase>(SecretGuardianClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

		if (Guardian)
		{
			// Buscamos su componente de vida y nos suscribimos a su muerte
			UHealthComponent* HealthComp = Guardian->FindComponentByClass<UHealthComponent>();
			if (HealthComp)
			{
				HealthComp->OnDeath.AddDynamic(this, &AOrchestratorFacade::ReportGuardianDefeated);
			}
		}
	}
}

void AOrchestratorFacade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPuzzleActive)
	{
		PuzzleTimeRemaining -= DeltaTime;

		// Actualizamos el HUD en cada frame
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ABulletRushHUD* HUD = Cast<ABulletRushHUD>(PC->GetHUD()))
			{
				HUD->SetCountdown(PuzzleTimeRemaining);
			}
		}
	}
}