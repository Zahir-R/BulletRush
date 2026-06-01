#include "Enemies/State/BossStateDead.h"

UBossStateDead::UBossStateDead()
{
}

void UBossStateDead::EnterState(ABossBase* Boss)
{
}

void UBossStateDead::ExitState(ABossBase* Boss)
{
}

FName UBossStateDead::GetStateTagName() const
{
	return "Dead";
}
