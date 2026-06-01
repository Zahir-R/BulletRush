#include "Combat/Commands/SpawnChargerCommand.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"

void USpawnChargerCommand::Execute(ASerXBoss* Boss)
{
	if (Boss)
	{
		Boss->DoSpawnCharger();
	}
}
