// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "SurveillanceDrone.generated.h"

class UStealthVisionComponent;
class UFloatingPawnMovement;
class USphereComponent;
/**
 * 
 */
UCLASS()
class BULLETRUSH_API ASurveillanceDrone : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	ASurveillanceDrone();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStealthVisionComponent* VisionComp;

	// Representación visual del cono de visión
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* VisionConeMesh;

	// Requisito estricto para que un APawn pueda usar NavMesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* MovementComp;

	// Array de puntos (TargetPoints o Actores vacíos) para definir la ruta
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Patrol")
	TArray<AActor*> PatrolPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

protected:
	virtual void BeginPlay() override;
};
