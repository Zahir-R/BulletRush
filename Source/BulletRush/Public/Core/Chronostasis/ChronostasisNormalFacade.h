#pragma once

#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "Core/Chronostasis/WaveConfig.h"
#include "Core/BulletRushGameInstance.h"
#include "ChronostasisNormalFacade.generated.h"

class APortalTrigger;
class UChronostasisWaveManager;
class UChronostasisSlowSystem;
class URequirementManager;
class UHealthComponent;
class AEnemyBase;

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
	FVector SecretPortalLocation = FVector(300.0f, 300.0f, 300.0f);

	UPROPERTY(EditAnywhere, Category = "Portals")
	FVector BossPortalLocation = FVector(500.0f, 500.0f, 500.0f);

private:
	void OnAllWavesComplete();
	void ActivateBossPortal();
	void ActivateSecretPortal();

	UFUNCTION()
	void OnBossPortalTriggered();

	UFUNCTION()
	void OnSecretPortalTriggered();

	void TravelToMap(FName MapName, ELevelState NextState);

	UPROPERTY()
	UChronostasisWaveManager* WaveManager;

	UPROPERTY()
	UChronostasisSlowSystem* SlowSystem;

	UPROPERTY()
	TWeakObjectPtr<URequirementManager> RequirementManagerRef;

	UPROPERTY()
	TWeakObjectPtr<UHealthComponent> PlayerHealthComp;

	UPROPERTY()
	APortalTrigger* PortalBossTrigger = nullptr;

	UPROPERTY()
	APortalTrigger* PortalSecretTrigger = nullptr;

	UFUNCTION()
	void OnPlayerHealthChanged(float NewHealth);

	UPROPERTY(EditAnywhere, Category = "Waves")
	TArray<FWaveConfig> Waves;

	bool bPlayerTookDamage = false;
	bool bPortalsActivated = false;
};
