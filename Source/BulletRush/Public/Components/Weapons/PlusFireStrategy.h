#pragma once

#include "CoreMinimal.h"
#include "WeaponsInterface.h"
#include "Sound/SoundBase.h"
#include "PlusFireStrategy.generated.h"

class USoundBase;
UCLASS()
class BULLETRUSH_API UPlusFireStrategy
	: public UObject,
	public IWeaponsInterface
{
	GENERATED_BODY()

public:
	UPlusFireStrategy();

	float SpreadAngle = 2.f;

	virtual void StartFiring(
		UWeaponBaseComponent* Weapon
	) override;

	virtual void StopFiring(
		UWeaponBaseComponent* Weapon
	) override;

	virtual void ExecuteFire(
		UWeaponBaseComponent* Weapon
	) override;

	float NewFireRate = .5f;
	float AdditionalDamage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* FireSound;
};