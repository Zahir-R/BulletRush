#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicManagerSubsystem.generated.h"

class UAudioComponent;
class USoundBase;

UCLASS()
class BULLETRUSH_API UMusicManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMusicManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void PlaySong(USoundBase* Song, float StartTime, float FadeInTime = 0.0f, bool bEnableLoopFade = false);
	void Stop(float FadeOutTime = 0.5f);
	void TransitionTo(USoundBase* NewSong, float FadeOutDuration = 3.0f,
	                  float FadeInDuration = 0.5f, float StartTime = 0.0f, bool bEnableLoopFade = false);

	void NotifyLevelTravel();
	void SavePlaybackPosition();
	bool IsPositionSaved() const { return bPendingResume; }
	float ConsumeSavedPosition();

	bool IsPlaying() const { return bIsPlaying; }

private:
	void EnsureAudioComponent();
	void StartSong(USoundBase* Song, float StartTime, float FadeInTime);
	float GetCurrentPlaybackPosition() const;
	void StartLoopFadeAfterDelay(float Delay);
	void BeginLoopFadeTimer();
	void UpdateLoopFade();
	void StopLoopFadeTimer();
	UFUNCTION()
	void OnTransitionComplete();

	UPROPERTY()
	UAudioComponent* AudioComponent;

	double PlaybackStartRealTime;
	float PlaybackStartTime;
	float SongDuration;
	float SavedPlaybackPosition;
	float SavedPreTransitionPosition;
	bool bPendingResume;
	bool bIsPlaying;
	bool bLoopFadeEnabled;

	FTimerHandle TransitionTimerHandle;
	FTimerHandle LoopFadeTimerHandle;
	UPROPERTY()
	USoundBase* PendingSong;
	float PendingFadeIn;
	float PendingStartTime;
};
