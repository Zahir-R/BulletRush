#pragma once

#include "CoreMinimal.h"
#include "WeaponsInterface.h"
#include "PlusFireStrategy.generated.h"

UCLASS()
class BULLETRUSH_API UPlusFireStrategy
	: public UObject,
	public IWeaponsInterface
{
	GENERATED_BODY()

public:
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
};