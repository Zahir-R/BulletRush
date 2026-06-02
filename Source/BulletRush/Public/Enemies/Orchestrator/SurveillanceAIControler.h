// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SurveillanceAIControler.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API ASurveillanceAIControler : public AAIController
{
	GENERATED_BODY()
	
public:
	void StartPatrolling(const TArray<AActor*>& Points);

protected:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	TArray<AActor*> Route;

	int32 CurrentIndex = 0;
	int32 PatrolDirection = 1;

	// Temporizador para romper recursiones y crear pausas de patrulla
	FTimerHandle PatrolTimerHandle;

	void GoToNextPoint();
	void CalculateNextIndex();
};
