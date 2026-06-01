#pragma once

#include "CoreMinimal.h"
#include "WeaponsInterface.h"
#include "AutoFireStrategy.generated.h"

UCLASS()
class BULLETRUSH_API UAutoFireStrategy : public UObject, public IWeaponsInterface
{
	GENERATED_BODY()

public:

	virtual void StartFiring(UWeaponBaseComponent* Weapon) override;

	virtual void StopFiring(UWeaponBaseComponent* Weapon) override;

	virtual void ExecuteFire(UWeaponBaseComponent* Weapon) override;
};