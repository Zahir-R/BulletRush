#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Chronostasis/WaveConfig.h"
#include "ChronostasisSecretFacade.generated.h"

class ALevelPortal;
class UChronostasisWaveManager;
class UChronostasisSlowSystem;
class AEnemyBase;
class USoundBase;

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
	float TimeLimit = 120.0f;

private:
	void OnAllWavesComplete();
	void OnTimeUp();
	void TickTimer();

	void OnBeforePortalTravel();

	UFUNCTION()
	void OnAutoTeleportToBoss();

	UPROPERTY()
	UChronostasisWaveManager* WaveManager;

	UPROPERTY()
	UChronostasisSlowSystem* SlowSystem;

	UPROPERTY()
	ALevelPortal* PortalToBoss = nullptr;

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* AmbientSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* CombatSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	float CombatStartOffset = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Waves")
	TArray<FWaveConfig> Waves;

	FTimerHandle CountdownTimerHandle;
	FTimerHandle AutoTeleportTimer;
	float TimeRemaining = 120.0f;
	bool bLevelComplete = false;
};
