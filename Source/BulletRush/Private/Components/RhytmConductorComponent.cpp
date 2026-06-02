#include "Components/RhytmConductorComponent.h"

URhytmConductorComponent::URhytmConductorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	BPM = 120.0f;
	bToggleStrongBeat = true;
}


void URhytmConductorComponent::StartRhythm(float InBPM)
{
	BPM = InBPM;
	float TimeBetweenBeats = 60.0f / BPM;

	// Configuramos el timer para que se repita al ritmo exacto
	GetWorld()->GetTimerManager().SetTimer(RhytmTimerHandle, this, &URhytmConductorComponent::BroadcastBeat, TimeBetweenBeats, true);
}

void URhytmConductorComponent::StopRhythm()
{
	GetWorld()->GetTimerManager().ClearTimer(RhytmTimerHandle);
}
/*
void URhytmConductorComponent::TriggerBeat()
{
	bToggleStrongBeat = !bToggleStrongBeat;
	OnBeatHit.Broadcast(bToggleStrongBeat);
}*/

void URhytmConductorComponent::BroadcastBeat()
{
	OnBeat.Broadcast();
}

void URhytmConductorComponent::TriggerSilence(bool bActivateSilence)
{
	OnSilence.Broadcast(bActivateSilence);
	if (bActivateSilence)
	{
		StopRhythm(); // Detenemos los ataques durante el silencio
	}
	else
	{
		StartRhythm(BPM); // Retomamos el ritmo
	}
}