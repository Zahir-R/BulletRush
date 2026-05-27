#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponsInterface.generated.h"

UINTERFACE(MinimalAPI)
class UWeaponsInterface : public UInterface
{
	GENERATED_BODY()
};

class BULLETRUSH_API IWeaponsInterface
{
	GENERATED_BODY()

public:

	virtual void StartFiring(
		class UWeaponBaseComponent* Weapon
	) = 0;

	virtual void StopFiring(
		class UWeaponBaseComponent* Weapon
	) = 0;

	virtual void ExecuteFire(
		class UWeaponBaseComponent* Weapon
	) = 0;
};