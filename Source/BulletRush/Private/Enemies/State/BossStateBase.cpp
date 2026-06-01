#include "Enemies/State/BossStateBase.h"

UBossState::UBossState()
{
}

void UBossState::EnterState(ABossBase* Boss)
{
}

void UBossState::UpdateState(ABossBase* Boss, float DeltaTime)
{
}

void UBossState::ExitState(ABossBase* Boss)
{
}

FName UBossState::GetStateTagName() const
{
	return NAME_None;
}
