// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StealthVisionComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BulletSpawnerComponent.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Enemies/Orchestrator/SurveillanceDrone.h"
#include "Engine/World.h"

UStealthVisionComponent::UStealthVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	VisionDistance = 500.0f;
	FieldOfViewDegrees = 25.0f;
	bHasDetectedPlayer = false;
}

// Called when the game starts
void UStealthVisionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UStealthVisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bHasDetectedPlayer) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ASurveillanceDrone* Drone = Cast<ASurveillanceDrone>(GetOwner());

	// Verificamos que el jugador y el dron (con su malla) existan
	if (!PlayerPawn || !Drone || !Drone->MeshEnemy) return;

	FVector OriginLoc = Drone->MeshEnemy->GetComponentLocation();
	FVector OriginForward = Drone->MeshEnemy->GetForwardVector();

	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	float DistanceToPlayer = FVector::Dist(OriginLoc, PlayerLoc);

	if (DistanceToPlayer <= VisionDistance)
	{
		FVector DirToPlayer = (PlayerLoc - OriginLoc).GetSafeNormal();

		float DotProduct = FVector::DotProduct(OriginForward, DirToPlayer);
		float CosineFOV = FMath::Cos(FMath::DegreesToRadians(FieldOfViewDegrees));

		// Si está dentro del cono de visión
		if (DotProduct > CosineFOV)
		{
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());

			// Trazado para comprobar que no hay paredes tapando la vista
			bool bHitObstacle = GetWorld()->LineTraceSingleByChannel(Hit, OriginLoc, PlayerLoc, ECC_Visibility, Params);

			// Si NO golpeó ninguna pared (!bHitObstacle) O si lo que golpeó fue explícitamente el jugador...
			if (!bHitObstacle || Hit.GetActor() == PlayerPawn)
			{
				bHasDetectedPlayer = true;

				UBulletSpawnerComponent* Spawner = GetOwner()->FindComponentByClass<UBulletSpawnerComponent>();
				if (Spawner)
				{
					// FAttackStep: Tipo, Cantidad, Velocidad, Delay, Origen, TiempoEntreBalas, SpecialParam, Daño
					FAttackStep TrapStep(EAttackType::SurroundingBullets, 200, 800.0f, 0.0f, PlayerLoc, 0.0f, 0.0f, 60.0f);
					TrapStep.BulletScale = FVector(0.5f);
					Spawner->ExecuteSingleAttack(TrapStep);
				}

				// Informamos a la Fachada
				if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					if (GM->LevelFacade)
					{
						GM->LevelFacade->HandlePlayerDetected(OriginLoc);
					}
				}
				if (Drone) Drone->Die();
			}
		}
	}
}

