#pragma once

#include "CoreMinimal.h"
#include "Buffs/PowerUpBase.h"
#include "UltimatePowerUp.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API AUltimatePowerUp : public APowerUpBase
{
	GENERATED_BODY()

public:
	AUltimatePowerUp();

protected:
	UFUNCTION()
	void OnUltimateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
