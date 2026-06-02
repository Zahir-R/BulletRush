#include "Combat/Commands/MoveCommand.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"

void UMoveCommand::Execute(ASerXBoss* Boss)
{
	if (Boss)
	{
		Boss->MoveTo(TargetLocation);
	}
}
