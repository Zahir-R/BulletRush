// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "SecretGuardian.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API ASecretGuardian : public AEnemyBase
{
	GENERATED_BODY()

public:
	ASecretGuardian();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	class UCapsuleComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	class USkeletalMeshComponent* GuardianMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fisicas")
	class UPhysicsAsset* PhysicsAsset;

protected:
	void BeginPlay();
	
};
