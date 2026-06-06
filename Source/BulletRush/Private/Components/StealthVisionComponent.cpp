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
	if (!PlayerPawn || !GetOwner()) return;

	FVector OwnerLoc = GetOwner()->GetActorLocation();
	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	float DistanceToPlayer = FVector::Dist(OwnerLoc, PlayerLoc);

	if (DistanceToPlayer <= VisionDistance)
	{
		FVector DirToPlayer = (PlayerLoc - OwnerLoc).GetSafeNormal();
		FVector OwnerForward = GetOwner()->GetActorForwardVector();

		float DotProduct = FVector::DotProduct(OwnerForward, DirToPlayer);
		float CosineFOV = FMath::Cos(FMath::DegreesToRadians(FieldOfViewDegrees));

		// Si está dentro del cono de visión
		if (DotProduct > CosineFOV)
		{
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());

			// Trazado para comprobar que no hay paredes tapando la vista
			if (GetWorld()->LineTraceSingleByChannel(Hit, OwnerLoc, PlayerLoc, ECC_Visibility, Params))
			{
				if (Hit.GetActor() == PlayerPawn)
				{
					bHasDetectedPlayer = true;

					UBulletSpawnerComponent* Spawner = GetOwner()->FindComponentByClass<UBulletSpawnerComponent>();
					if (Spawner)
					{
						// FAttackStep: Tipo, Cantidad, Velocidad, Delay, Origen, TiempoEntreBalas, SpecialParam, Daño
						// Le pasamos PlayerLoc para que el origen del ataque sea el jugador y lo rodee
						FAttackStep TrapStep(EAttackType::SurroundingBullets, 200, 800.0f, 0.0f, PlayerLoc, 0.0f, 0.0f, 60.0f);
						TrapStep.BulletScale = FVector(0.5f);
						Spawner->ExecuteSingleAttack(TrapStep);
					}

					// Informamos a la Fachada a través del GameMode (Patrón Observer indirecto)
					if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
					{
						if (GM->LevelFacade)
						{
							GM->LevelFacade->HandlePlayerDetected(OwnerLoc);
						}
					}
					ASurveillanceDrone* Drone = Cast<ASurveillanceDrone>(GetOwner());
					Drone->Die();
				}
			}
		}
	}
}

