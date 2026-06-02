// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/SurveillanceAIControler.h"
#include "Enemies/Orchestrator/SurveillanceDrone.h"
#include "TimerManager.h"

void ASurveillanceAIControler::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ASurveillanceDrone* Drone = Cast<ASurveillanceDrone>(InPawn))
	{
		StartPatrolling(Drone->PatrolPoints);
	}
}

void ASurveillanceAIControler::StartPatrolling(const TArray<AActor*>& Points)
{
	if (Points.Num() == 0) return;

	Route = Points;
	CurrentIndex = 0;
	PatrolDirection = 1;

	GoToNextPoint();
}

void ASurveillanceAIControler::GoToNextPoint()
{
	if (Route.Num() == 0) return;

	AActor* Target = Route[CurrentIndex];
	if (Target)
	{
		SetFocus(Target);
		// Orden limpia y estándar. Radio de 50.0f
		MoveToLocation(Target->GetActorLocation(), 50.0f);
	}
}

void ASurveillanceAIControler::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// Ya sea que llegó con éxito, o falló porque un obstáculo se cruzó,
	// avanzamos al siguiente punto tras una breve pausa.
	if (Route.Num() > 1)
	{
		// 1. Calculamos a dónde ir ahora
		CalculateNextIndex();

		// 2. Esperamos 1 segundo antes de movernos.
		// Esto EVITA el Stack Overflow y le da un toque natural al sigilo.
		GetWorld()->GetTimerManager().SetTimer(PatrolTimerHandle, this, &ASurveillanceAIControler::GoToNextPoint, 1.0f, false);
	}
}

void ASurveillanceAIControler::CalculateNextIndex()
{
	CurrentIndex += PatrolDirection;

	if (CurrentIndex >= Route.Num() - 1)
	{
		CurrentIndex = Route.Num() - 1;
		PatrolDirection = -1;
	}
	else if (CurrentIndex <= 0)
	{
		CurrentIndex = 0;
		PatrolDirection = 1;
	}
}