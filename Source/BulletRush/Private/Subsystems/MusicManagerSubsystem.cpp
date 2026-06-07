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
	AudioComponent = nullptr;
	PlaybackStartRealTime = 0.0;
	PlaybackStartTime = 0.0f;
	SongDuration = 0.0f;
	SavedPlaybackPosition = 0.0f;
	SavedPreTransitionPosition = 0.0f;
	bPendingResume = false;
	bIsPlaying = false;
	bLoopFadeEnabled = false;
	PendingSong = nullptr;
	PendingFadeIn = 0.0f;
	PendingStartTime = 0.0f;
}

void UMusicManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AudioComponent = NewObject<UAudioComponent>(this);
	AudioComponent->bIsUISound = true;
	AudioComponent->bAutoDestroy = false;
	AudioComponent->bStopWhenOwnerDestroyed = false;

	bIsPlaying = false;
	bPendingResume = false;
	SavedPlaybackPosition = 0.0f;
	PlaybackStartRealTime = 0.0;
}

void UMusicManagerSubsystem::Deinitialize()
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent = nullptr;
	}
	Super::Deinitialize();
}

void UMusicManagerSubsystem::EnsureAudioComponent()
{
	if (AudioComponent) return;

	AudioComponent = NewObject<UAudioComponent>(this);
	AudioComponent->bIsUISound = true;
	AudioComponent->bAutoDestroy = false;
	AudioComponent->bStopWhenOwnerDestroyed = false;
}

float UMusicManagerSubsystem::GetCurrentPlaybackPosition() const
{
	if (!bIsPlaying || PlaybackStartRealTime <= 0.0) return 0.0f;

	double Elapsed = FPlatformTime::Seconds() - PlaybackStartRealTime;
	float Position = PlaybackStartTime + static_cast<float>(Elapsed);

	if (SongDuration > 0.0f && Position >= SongDuration) Position = FMath::Fmod(Position, SongDuration);

	return FMath::Max(0.0f, Position);
}

void UMusicManagerSubsystem::NotifyLevelTravel()
{
	if (bIsPlaying)
	{
		if (SavedPreTransitionPosition > 0.0f)
		{
			SavedPlaybackPosition = SavedPreTransitionPosition;
			SavedPreTransitionPosition = 0.0f;
		}
		else SavedPlaybackPosition = GetCurrentPlaybackPosition();
		
		bPendingResume = (SavedPlaybackPosition > 0.0f);
	}
	else
	{
		SavedPlaybackPosition = 0.0f;
		bPendingResume = false;
	}

	StopLoopFadeTimer();
	if (AudioComponent)
	{
		AudioComponent->SetVolumeMultiplier(1.0f);
		AudioComponent->Stop();
		AudioComponent = nullptr;
	}
	bIsPlaying = false;
	PlaybackStartRealTime = 0.0;
}

void UMusicManagerSubsystem::SavePlaybackPosition()
{
	if (bIsPlaying)
	{
		if (SavedPreTransitionPosition > 0.0f)
		{
			SavedPlaybackPosition = SavedPreTransitionPosition;
			SavedPreTransitionPosition = 0.0f;
		}
		else SavedPlaybackPosition = GetCurrentPlaybackPosition();
		
		bPendingResume = (SavedPlaybackPosition > 0.0f);
	}
	else UE_LOG(LogTemp, Warning, TEXT("[MusicManager] NOT saving — not playing"));
}

void UMusicManagerSubsystem::PlaySong(USoundBase* Song, float StartTime, float FadeInTime, bool bEnableLoopFade)
{
	if (!Song) return;
	EnsureAudioComponent();

	bPendingResume = false;
	SavedPlaybackPosition = 0.0f;
	SavedPreTransitionPosition = 0.0f;
	bLoopFadeEnabled = bEnableLoopFade;

	StartSong(Song, StartTime, FadeInTime);
	bIsPlaying = true;
}

void UMusicManagerSubsystem::Stop(float FadeOutTime)
{
	if (!AudioComponent) return;

	AudioComponent->FadeOut(FadeOutTime, 0.0f);
	bIsPlaying = false;
	bPendingResume = false;
	SavedPlaybackPosition = 0.0f;
	SavedPreTransitionPosition = 0.0f;
	PlaybackStartRealTime = 0.0;

	PendingSong = nullptr;
	StopLoopFadeTimer();
	UWorld* World = GetWorld();
	if (World) World->GetTimerManager().ClearTimer(TransitionTimerHandle);
}

void UMusicManagerSubsystem::TransitionTo(USoundBase* NewSong, float FadeOutDuration, float FadeInDuration, float StartTime, bool bEnableLoopFade)
{
	if (!NewSong) return;
	EnsureAudioComponent();

	StopLoopFadeTimer();
	SavedPreTransitionPosition = GetCurrentPlaybackPosition();

	bLoopFadeEnabled = bEnableLoopFade;
	PendingSong = NewSong;
	PendingFadeIn = FadeInDuration;
	PendingStartTime = StartTime;

	AudioComponent->FadeOut(FadeOutDuration, 0.0f);

	UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	
	if (World) World->GetTimerManager().SetTimer(TransitionTimerHandle, this, &UMusicManagerSubsystem::OnTransitionComplete, FadeOutDuration, false);
	else OnTransitionComplete();
}

float UMusicManagerSubsystem::ConsumeSavedPosition()
{
	float Pos = SavedPlaybackPosition;
	bPendingResume = false;
	SavedPlaybackPosition = 0.0f;
	return Pos;
}

void UMusicManagerSubsystem::StartSong(USoundBase* Song, float StartTime, float FadeInTime)
{
	if (!AudioComponent || !Song) return;

	StopLoopFadeTimer();
	AudioComponent->Stop();
	AudioComponent->SetSound(Song);

	PlaybackStartTime = StartTime;
	PlaybackStartRealTime = FPlatformTime::Seconds();

	if (USoundWave* Wave = Cast<USoundWave>(Song))
	{
		Wave->bLooping = true;
		SongDuration = static_cast<float>(Wave->GetDuration());
	}
	else SongDuration = 0.0f;

	if (FadeInTime > 0.0f) AudioComponent->FadeIn(FadeInTime, 1.0f, StartTime);
	else AudioComponent->Play(StartTime);

	if (bLoopFadeEnabled)
		StartLoopFadeAfterDelay(FadeInTime);
}

void UMusicManagerSubsystem::StartLoopFadeAfterDelay(float Delay)
{
	StopLoopFadeTimer();
	if (SongDuration <= 0.0f) return;

	UWorld* World = GetWorld();
	if (!World) return;

	if (Delay > 0.0f)
	{
		World->GetTimerManager().SetTimer(LoopFadeTimerHandle, this,
			&UMusicManagerSubsystem::BeginLoopFadeTimer, Delay, false);
	}
	else
	{
		BeginLoopFadeTimer();
	}
}

void UMusicManagerSubsystem::BeginLoopFadeTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(LoopFadeTimerHandle, this,
		&UMusicManagerSubsystem::UpdateLoopFade, 0.1f, true);

	UpdateLoopFade();
}

void UMusicManagerSubsystem::StopLoopFadeTimer()
{
	UWorld* World = GetWorld();
	if (World) World->GetTimerManager().ClearTimer(LoopFadeTimerHandle);
}

void UMusicManagerSubsystem::UpdateLoopFade()
{
	if (!AudioComponent || !bIsPlaying || SongDuration <= 0.0f)
	{
		if (AudioComponent)
			AudioComponent->SetVolumeMultiplier(1.0f);
		return;
	}

	float Pos = GetCurrentPlaybackPosition();
	const float FadeDuration = 5.0f;

	if (Pos >= SongDuration - FadeDuration)
	{
		float Alpha = (Pos - (SongDuration - FadeDuration)) / FadeDuration;
		AudioComponent->SetVolumeMultiplier(FMath::Lerp(1.0f, 0.0f, Alpha));
	}
	else if (Pos < FadeDuration)
	{
		float Alpha = Pos / FadeDuration;
		AudioComponent->SetVolumeMultiplier(FMath::Lerp(0.0f, 1.0f, Alpha));
	}
	else
	{
		AudioComponent->SetVolumeMultiplier(1.0f);
	}
}

void UMusicManagerSubsystem::OnTransitionComplete()
{
	if (!PendingSong) return;
	EnsureAudioComponent();

	StartSong(PendingSong, PendingStartTime, PendingFadeIn);
	bIsPlaying = true;

	PendingSong = nullptr;
}
