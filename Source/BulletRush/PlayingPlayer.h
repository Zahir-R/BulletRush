// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayingPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

UCLASS()
class BULLETRUSH_API APlayingPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	APlayingPlayer();

	USpringArmComponent* CameraBoom;
	UCameraComponent* FollowCamera;
	UStaticMeshComponent* VisualMesh;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Val);
	void MoveRight(float Val);
	void MoveUp(float Val);
};
