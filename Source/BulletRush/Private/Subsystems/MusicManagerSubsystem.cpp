#include "Subsystems/MusicManagerSubsystem.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "HAL/PlatformTime.h"

UMusicManagerSubsystem::UMusicManagerSubsystem()
{
	SavedPlaybackPosition = 0.0f;
	bPendingResume = false;
}

void UMusicManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GetOrCreateChannel(FName("Music"));
}

void UMusicManagerSubsystem::Deinitialize()
{
	for (UAudioComponent* Comp : ChannelComponents)
	{
		if (Comp) Comp->Stop();
	}
	ChannelComponents.Empty();
	ChannelIndex.Empty();
	ChannelState.Empty();
	Super::Deinitialize();
}

int32 UMusicManagerSubsystem::GetOrCreateChannel(FName ChannelName)
{
	if (int32* Idx = ChannelIndex.Find(ChannelName))
		return *Idx;

	int32 NewIdx = ChannelComponents.Num();
	UAudioComponent* Comp = NewObject<UAudioComponent>(this);
	Comp->bIsUISound = true;
	Comp->bAutoDestroy = false;
	Comp->bStopWhenOwnerDestroyed = false;

	ChannelComponents.Add(Comp);
	ChannelIndex.Add(ChannelName, NewIdx);
	ChannelState.Add(ChannelName, FMusicChannelState());
	return NewIdx;
}

UAudioComponent* UMusicManagerSubsystem::GetChannelComponent(FName ChannelName) const
{
	const int32* Idx = ChannelIndex.Find(ChannelName);
	if (!Idx || !ChannelComponents.IsValidIndex(*Idx)) return nullptr;
	return ChannelComponents[*Idx];
}

void UMusicManagerSubsystem::PlayOnChannel(FName ChannelName, USoundBase* Song, float StartTime, float FadeInTime, bool bEnableLoopFade)
{
	if (!Song) return;

	GetOrCreateChannel(ChannelName);
	UAudioComponent* Comp = GetChannelComponent(ChannelName);
	if (!Comp) return;

	FMusicChannelState& State = ChannelState.FindOrAdd(ChannelName);
	State.bLoopFadeEnabled = bEnableLoopFade;

	Comp->Stop();
	Comp->SetSound(Song);
	Comp->SetVolumeMultiplier(1.0f);

	State.StartTime = StartTime;
	State.StartRealTime = FPlatformTime::Seconds();

	if (USoundWave* Wave = Cast<USoundWave>(Song))
	{
		Wave->bLooping = true;
		State.SongDuration = static_cast<float>(Wave->GetDuration());
	}
	else
	{
		State.SongDuration = 0.0f;
	}

	if (FadeInTime > 0.0f)
		Comp->FadeIn(FadeInTime, 1.0f, StartTime);
	else
		Comp->Play(StartTime);

	if (bEnableLoopFade && State.SongDuration > 0.0f)
		StartLoopFadeTimer();
}

void UMusicManagerSubsystem::StopChannel(FName ChannelName, float FadeOutTime)
{
	UAudioComponent* Comp = GetChannelComponent(ChannelName);
	if (!Comp) return;

	if (FadeOutTime > 0.0f)
		Comp->FadeOut(FadeOutTime, 0.0f);
	else
		Comp->Stop();

	if (FMusicChannelState* State = ChannelState.Find(ChannelName))
	{
		State->bLoopFadeEnabled = false;
	}
}

void UMusicManagerSubsystem::FadeChannelVolume(FName ChannelName, float FadeDuration, float TargetVolume)
{
	UAudioComponent* Comp = GetChannelComponent(ChannelName);
	if (!Comp) return;

	if (TargetVolume <= 0.0f)
	{
		Comp->FadeOut(FadeDuration, 0.0f);
	}
	else
	{
		Comp->SetVolumeMultiplier(TargetVolume);
	}

	if (FMusicChannelState* State = ChannelState.Find(ChannelName))
	{
		State->Volume = TargetVolume;
	}
}

float UMusicManagerSubsystem::GetChannelPosition(FName ChannelName) const
{
	const FMusicChannelState* State = ChannelState.Find(ChannelName);
	if (!State || State->StartRealTime <= 0.0) return 0.0f;

	double Elapsed = FPlatformTime::Seconds() - State->StartRealTime;
	float Position = State->StartTime + static_cast<float>(Elapsed);

	if (State->SongDuration > 0.0f && Position >= State->SongDuration)
		Position = FMath::Fmod(Position, State->SongDuration);

	return FMath::Max(0.0f, Position);
}

bool UMusicManagerSubsystem::IsChannelPlaying(FName ChannelName) const
{
	UAudioComponent* Comp = GetChannelComponent(ChannelName);
	return Comp && Comp->IsPlaying();
}

bool UMusicManagerSubsystem::IsPlaying() const
{
	return IsChannelPlaying(FName("Music"));
}

void UMusicManagerSubsystem::PlaySong(USoundBase* Song, float StartTime, float FadeInTime, bool bEnableLoopFade)
{
	if (!Song) return;

	bPendingResume = false;
	SavedPlaybackPosition = 0.0f;

	StopChannel(FName("Overlay"));

	PlayOnChannel(FName("Music"), Song, StartTime, FadeInTime, bEnableLoopFade);
}

void UMusicManagerSubsystem::Stop(float FadeOutTime)
{
	for (auto& Pair : ChannelIndex)
	{
		StopChannel(Pair.Key, FadeOutTime);
	}
	bPendingResume = false;
	SavedPlaybackPosition = 0.0f;
	StopLoopFadeTimer();
}

void UMusicManagerSubsystem::TransitionTo(USoundBase* NewSong, float FadeOutDuration, float FadeInDuration, float StartTime, bool bEnableLoopFade)
{
	if (!NewSong) return;

	SavedPlaybackPosition = GetChannelPosition(FName("Music"));
	bPendingResume = true;

	FadeChannelVolume(FName("Music"), FadeOutDuration, 0.0f);

	PlayOnChannel(FName("Overlay"), NewSong, StartTime, FadeInDuration, bEnableLoopFade);
}

void UMusicManagerSubsystem::SavePlaybackPosition()
{
	float MusicPos = GetChannelPosition(FName("Music"));
	if (MusicPos > 0.0f)
	{
		SavedPlaybackPosition = MusicPos;
		bPendingResume = true;
	}
}

float UMusicManagerSubsystem::ConsumeSavedPosition()
{
	float Pos = SavedPlaybackPosition;
	bPendingResume = false;
	SavedPlaybackPosition = 0.0f;
	return Pos;
}

void UMusicManagerSubsystem::NotifyLevelTravel()
{
	SavePlaybackPosition();

	StopLoopFadeTimer();

	for (UAudioComponent* Comp : ChannelComponents)
	{
		if (Comp)
		{
			Comp->SetVolumeMultiplier(1.0f);
			Comp->Stop();
		}
	}
	ChannelComponents.Empty();
	ChannelIndex.Empty();
	ChannelState.Empty();
}

void UMusicManagerSubsystem::StartLoopFadeTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!World->GetTimerManager().IsTimerActive(GlobalLoopFadeTimerHandle))
	{
		World->GetTimerManager().SetTimer(GlobalLoopFadeTimerHandle, this,
			&UMusicManagerSubsystem::OnLoopFadeTick, 0.1f, true);
	}
}

void UMusicManagerSubsystem::StopLoopFadeTimer()
{
	UWorld* World = GetWorld();
	if (World)
		World->GetTimerManager().ClearTimer(GlobalLoopFadeTimerHandle);
}

void UMusicManagerSubsystem::OnLoopFadeTick()
{
	const float FadeDuration = 5.0f;
	bool bAnyActive = false;

	for (auto& Pair : ChannelIndex)
	{
		FMusicChannelState* State = ChannelState.Find(Pair.Key);
		if (!State || !State->bLoopFadeEnabled || State->SongDuration <= 0.0f)
			continue;

		UAudioComponent* Comp = GetChannelComponent(Pair.Key);
		if (!Comp || !Comp->IsPlaying())
			continue;

		bAnyActive = true;
		float Pos = GetChannelPosition(Pair.Key);

		if (Pos >= State->SongDuration - FadeDuration)
		{
			float Alpha = (Pos - (State->SongDuration - FadeDuration)) / FadeDuration;
			Comp->SetVolumeMultiplier(FMath::Lerp(1.0f, 0.0f, Alpha));
		}
		else if (Pos < FadeDuration)
		{
			float Alpha = Pos / FadeDuration;
			Comp->SetVolumeMultiplier(FMath::Lerp(0.0f, 1.0f, Alpha));
		}
		else
		{
			Comp->SetVolumeMultiplier(1.0f);
		}
	}

	if (!bAnyActive)
		StopLoopFadeTimer();
}
