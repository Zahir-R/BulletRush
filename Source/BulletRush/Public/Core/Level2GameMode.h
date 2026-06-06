#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Level2GameMode.generated.h"

class ALevel21Facade;
class ALevel2SFacade;
class AVaultKeeperFacade;

UCLASS()
class BULLETRUSH_API ALevel2GameMode : public AGameMode
{
    GENERATED_BODY()

public:
    ALevel2GameMode();

protected:
    virtual void BeginPlay() override;
    UFUNCTION()
    void OnPlayerDeath();
private:
    void DetectAndActivateFacade();


    UPROPERTY()
    ALevel21Facade* Facade21 = nullptr;

    UPROPERTY()
    ALevel2SFacade* Facade2S = nullptr;

    UPROPERTY()
    AVaultKeeperFacade* FacadeVK = nullptr;  
};