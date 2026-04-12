// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Components/BulletSpawnerComponent.h"
#include "TopDownPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class BULLETRUSH_API ATopDownPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATopDownPlayer();

	USpringArmComponent* CameraBoom;
	UCameraComponent* FollowCamera;
	UStaticMeshComponent* VisualMesh;

	UBulletSpawnerComponent* Spawner;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Val);
	void MoveRight(float Val);
	void MoveUp(float Val);
};
