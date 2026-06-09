#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChronostasisBossFacade.generated.h"

class ASerXBoss;
class UChronostasisFactoryEnemy;
class UGenericEnemyFactory;
class AEnemyBase;
class USoundBase;

UCLASS()
class BULLETRUSH_API AChronostasisBossFacade : public AActor
{
	GENERATED_BODY()

public:
	AChronostasisBossFacade();

protected:
	virtual void BeginPlay() override;

public:
	void StartLevel();

	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<ASerXBoss> SerXBossClass;

	UPROPERTY(EditAnywhere, Category = "Boss")
	FVector BossSpawnLocation = FVector(500.0f, 500.0f, 300.0f);

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* AmbientSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* CombatSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	float CombatStartOffset = 11.3f;

private:
	void SpawnBoss();

	UFUNCTION()
	void OnBossDeath(AEnemyBase* DeadEnemy);

	UPROPERTY()
	ASerXBoss* BossInstance = nullptr;

	bool bLevelComplete = false;
};
