// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelPortal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class BULLETRUSH_API ALevelPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelPortal();

	FName TargetLevelName; // El nombre del nivel al que quieres teletransportar a la nave

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// for detectar overlaps
	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	UStaticMeshComponent* PortalMesh;

	// Función que se ejecuta al chocar
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
