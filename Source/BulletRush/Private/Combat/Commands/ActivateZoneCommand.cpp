#include "Combat/Commands/ActivateZoneCommand.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"

void UActivateZoneCommand::Execute(ASerXBoss* Boss)
{
	if (Boss)
	{
		Boss->ActivateZone();
	}
}
