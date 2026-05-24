#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpManager.generated.h"

class UBoxComponent;
class APowerUpBase;

UCLASS()
class BULLETRUSH_API APowerUpManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerUpManager();

	void OnPowerUpCollected(APowerUpBase* PowerUp);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<TSubclassOf<class APowerUpBase>> PowerUpClasses;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MinSpawnTime = 30.0f;
	
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MaxSpawnTime = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 MaxPowerUps = 5;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float PowerUpLifetime = 50.0f;

	UPROPERTY()
	TArray<APowerUpBase*> SpawnedPowerUps;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* SpawnArea;

	UFUNCTION()
	void SpawnRandomPowerUp();

	FTimerHandle SpawnTimer;
	void ScheduleNextSpawn();
};
