#pragma once

#include "CoreMinimal.h"
#include "WeaponsInterface.h"
#include "Sound/SoundBase.h"
#include "AutoFireStrategy.generated.h"

class USoundBase;
UCLASS()
class BULLETRUSH_API UAutoFireStrategy : public UObject, public IWeaponsInterface
{
	GENERATED_BODY()

public:
	UAutoFireStrategy();

	virtual void StartFiring(UWeaponBaseComponent* Weapon) override;

	virtual void StopFiring(UWeaponBaseComponent* Weapon) override;

	virtual void ExecuteFire(UWeaponBaseComponent* Weapon) override;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* FireSound;
};