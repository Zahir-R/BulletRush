#pragma once

#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "Core/Chronostasis/WaveConfig.h"
#include "Core/BulletRushGameInstance.h"
#include "ChronostasisNormalFacade.generated.h"

class ALevelPortal;
class UChronostasisWaveManager;
class UChronostasisSlowSystem;
class URequirementManager;
class UHealthComponent;
class AEnemyBase;
class USoundBase;

UCLASS()
class BULLETRUSH_API AChronostasisNormalFacade : public APublisher
{
	GENERATED_BODY()

public:
	AChronostasisNormalFacade();

protected:
	virtual void BeginPlay() override;

public:
	void StartLevel();

	UFUNCTION()
	void OnEnemyKilled(AEnemyBase* Enemy);

	UPROPERTY(EditAnywhere, Category = "Portals")
	FVector PortalLocation = FVector(500.0f, 500.0f, 500.0f);

private:
	void OnAllWavesComplete();

	UPROPERTY()
	UChronostasisWaveManager* WaveManager;

	UPROPERTY()
	UChronostasisSlowSystem* SlowSystem;

	UPROPERTY()
	TWeakObjectPtr<URequirementManager> RequirementManagerRef;

	UPROPERTY()
	TWeakObjectPtr<UHealthComponent> PlayerHealthComp;

	UPROPERTY()
	ALevelPortal* PortalToBoss = nullptr;

	UFUNCTION()
	void OnPlayerHealthChanged(float NewHealth);

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* AmbientSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* CombatSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	float CombatStartOffset = 11.3f;

	UPROPERTY(EditAnywhere, Category = "Waves")
	TArray<FWaveConfig> Waves;

	bool bPlayerTookDamage = false;
};
