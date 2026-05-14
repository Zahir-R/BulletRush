// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BuffComponent.h"
#include "PlayingPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UWeaponBaseComponent;

UCLASS()
class BULLETRUSH_API APlayingPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	APlayingPlayer();

	USpringArmComponent* CameraBoom;
	UCameraComponent* FollowCamera;
	UStaticMeshComponent* VisualMesh;

	// TESTING
	UBulletSpawnerComponent* Spawner;
	
	void TestCircle();
	void TestSpiral();
	void TestBurst();
	

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Val);
	void MoveRight(float Val);
	void MoveUp(float Val);

	void OnFirePressed();
	void OnFireReleased();

	TArray<UWeaponBaseComponent*> EquippedWeapons;
	UWeaponBaseComponent* TestWeapon;
	UWeaponBaseComponent* TestWeapontwo;

	UHealthComponent* HealthComp;
	UBuffComponent* BuffComp;
};
