#include "../../Public/Components/RhytmConductorComponent.h"

URhytmConductorComponent::URhytmConductorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	BPM = 120.0f;
	bToggleStrongBeat = true;
}


void URhytmConductorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URhytmConductorComponent::StartMusic()
{
	float TimeBetweenBeats = 60.0f / BPM;

	GetWorld()->GetTimerManager().SetTimer(RhytmTimerHandle, this, &URhytmConductorComponent::TriggerBeat, TimeBetweenBeats, true);
}

void URhytmConductorComponent::TriggerBeat()
{
	bToggleStrongBeat = !bToggleStrongBeat;
	OnBeatHit.Broadcast(bToggleStrongBeat);
}

void URhytmConductorComponent::EnterSilence()
{
	GetWorld()->GetTimerManager().ClearTimer(RhytmTimerHandle);
	OnSilenceEnter.Broadcast();
}