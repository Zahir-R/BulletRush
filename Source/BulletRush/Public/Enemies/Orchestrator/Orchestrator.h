// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "Orchestrator.generated.h"

/**
 * 
 */

UCLASS()
class BULLETRUSH_API AOrchestrator : public ABossBase
{
	GENERATED_BODY()
	
public:
	AOrchestrator();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	class UCapsuleComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	class USkeletalMeshComponent* OrchestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fisicas")
	class UPhysicsAsset* PhysicsAsset;

protected:
	virtual void BeginPlay() override;
};
