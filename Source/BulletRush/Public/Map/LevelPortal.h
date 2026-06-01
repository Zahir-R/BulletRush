// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Subscriber.h"
#include "LevelPortal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ULevelRoutingSubsystem;

UCLASS()
class BULLETRUSH_API ALevelPortal : public AActor, public ISubscriber
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelPortal();
	// var para el nombre del nivel
	FName TargetLevelName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	UStaticMeshComponent* PortalMesh;

	// Funcion que se activa cuando el player choca con el box, para cargar el nivel correspondiente
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult
	);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// for detectar overlaps
	UPROPERTY(VisibleAnywhere, Category = "Componentes")
	UBoxComponent* CollisionBox;
	// ISubscriber implementation
	virtual void Update(class APublisher* Publisher) override;

};
