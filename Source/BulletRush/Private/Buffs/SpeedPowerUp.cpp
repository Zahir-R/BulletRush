#include "Buffs/SpeedPowerUp.h"
#include "Buffs/SpeedBoost.h"

ASpeedPowerUp::ASpeedPowerUp()
{
	BuffClass = USpeedBoost::StaticClass();
	BuffMagnitude = 2.0f;
	BuffDuration = 5.0f;
}