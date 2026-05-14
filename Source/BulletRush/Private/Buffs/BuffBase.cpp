#include "Buffs/BuffBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UBuffBase::Apply(AActor* InOwner)
{
	Owner = InOwner;
	if (Duration > 0.0f) StartDurationTimer();
}

void UBuffBase::Remove()
{
	if (Owner && Owner->GetWorld()) Owner->GetWorld()->GetTimerManager().ClearTimer(DurationTimer);
}

void UBuffBase::StartDurationTimer()
{
	if (Owner && Owner->GetWorld())
		Owner->GetWorld()->GetTimerManager().SetTimer(DurationTimer, this, &UBuffBase::Remove, Duration, false);
}