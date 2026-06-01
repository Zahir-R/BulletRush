#include "Combat/Commands/SpawnLinkerCommand.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"

void USpawnLinkerCommand::Execute(ASerXBoss* Boss)
{
	if (Boss)
	{
		Boss->DoSpawnLinker();
	}
}
