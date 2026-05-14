#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpManager.generated.h"

class UBoxComponent;

UCLASS()
class BULLETRUSH_API APowerUpManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerUpManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<TSubclassOf<class APowerUpBase>> PowerUpClasses;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MinSpawnTime = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float MaxSpawnTime = 10.0f;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* SpawnArea;

	UFUNCTION()
	void SpawnRandomPowerUp();

	FTimerHandle SpawnTimer;

};
