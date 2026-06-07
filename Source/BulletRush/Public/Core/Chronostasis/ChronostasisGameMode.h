#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ChronostasisGameMode.generated.h"

class AChronostasisNormalFacade;
class AChronostasisSecretFacade;
class AChronostasisBossFacade;

UCLASS()
class BULLETRUSH_API AChronostasisGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AChronostasisGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPlayerDeath();

private:
	void DetectAndActivateFacade();

	UPROPERTY()
	AChronostasisNormalFacade* FacadeNormal = nullptr;

	UPROPERTY()
	AChronostasisSecretFacade* FacadeSecret = nullptr;

	UPROPERTY()
	AChronostasisBossFacade* FacadeBoss = nullptr;
};
