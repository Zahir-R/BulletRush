// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalManager.generated.h"


USTRUCT()
struct FPortalData
{
	GENERATED_BODY()

	FName LevelName;
	FVector SpawnLocation;

	FPortalData() : LevelName(NAME_None), SpawnLocation(FVector::ZeroVector) {}
	FPortalData(FName InLevelName, FVector InLocation) : LevelName(InLevelName), SpawnLocation(InLocation) {}
};

UCLASS(Blueprintable)
class BULLETRUSH_API APortalManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnPortalesCupHead();
	void VolverCupHead(FVector Location);
private:

	TArray<FPortalData> PortalesDataBase;
	

};
