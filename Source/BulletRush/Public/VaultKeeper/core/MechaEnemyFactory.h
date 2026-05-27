// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechaEnemyFactory.generated.h"


UENUM(BlueprintType)
enum class EMechaEnemyType : uint8
{
	DroneMecha    UMETA(DisplayName = "Dron Mecha"),
	BatteryActor  UMETA(DisplayName = "Bateria de Soporte"),
	VaultKeeper   UMETA(DisplayName = "Jefe Vault-Keeper")
};

UCLASS()
class BULLETRUSH_API AMechaEnemyFactory : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMechaEnemyFactory();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "MechaFactory")
	AActor* CreateEnemy(EMechaEnemyType EnemyType, FVector Location, FRotator Rotation);

protected:
	// Clases asignables para los tipos de enemigos
	UPROPERTY(EditDefaultsOnly, Category = "MechaFactory|Classes")
	TSubclassOf<AActor> DronMechaClass;

	UPROPERTY(EditDefaultsOnly, Category = "MechaFactory|Classes")
	TSubclassOf<AActor> BatteryActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "MechaFactory|Classes")
	TSubclassOf<AActor> VaultKeeperClass;
};