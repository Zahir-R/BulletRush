#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Chronostasis/WaveConfig.h"
#include "ChronostasisSecretFacade.generated.h"

class APortalTrigger;
class UChronostasisWaveManager;
class UChronostasisSlowSystem;
class AEnemyBase;

UCLASS()
class BULLETRUSH_API AChronostasisSecretFacade : public AActor
{
	GENERATED_BODY()

public:
	AChronostasisSecretFacade();

protected:
	virtual void BeginPlay() override;

public:
	void StartLevel();

	UFUNCTION()
	void OnEnemyKilled(AEnemyBase* Enemy);

	UPROPERTY(EditAnywhere, Category = "Portals")
	FVector PortalLocation = FVector(0.0f, 0.0f, 100.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float TimeLimit = 10.0f;

private:
	void OnAllWavesComplete();
	void OnTimeUp();
	void TickTimer();

	UFUNCTION()
	void OnPortalToBossTriggered();

	UFUNCTION()
	void OnAutoTeleportToBoss();

	UPROPERTY()
	UChronostasisWaveManager* WaveManager;

	UPROPERTY()
	UChronostasisSlowSystem* SlowSystem;

	UPROPERTY()
	APortalTrigger* PortalBossTrigger = nullptr;

	UPROPERTY(EditAnywhere, Category = "Waves")
	TArray<FWaveConfig> Waves;

	FTimerHandle CountdownTimerHandle;
	FTimerHandle AutoTeleportTimer;
	float TimeRemaining = 120.0f;
	bool bLevelComplete = false;
};
