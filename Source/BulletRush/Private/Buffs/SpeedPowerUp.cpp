#include "Buffs/SpeedPowerUp.h"
#include "Buffs/Buff_SpeedBoost.h"

ASpeedPowerUp::ASpeedPowerUp()
{
	BuffClass = UBuff_SpeedBoost::StaticClass();
	BuffMagnitude = 2.0f;
	BuffDuration = 5.0f;
}