#include "Combat/Commands/AttackCommand.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"

void UAttackCommand::Execute(ASerXBoss* Boss)
{
	if (Boss)
	{
		Boss->ExecuteAttack(AttackPatternIndex);
	}
}
