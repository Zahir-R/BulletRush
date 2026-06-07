#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicManagerSubsystem.generated.h"

class UAudioComponent;
class USoundBase;

USTRUCT()
struct FMusicChannelState
{
	GENERATED_BODY()

	double StartRealTime = 0.0;
	float StartTime = 0.0f;
	float SongDuration = 0.0f;
	float Volume = 1.0f;
	bool bLoopFadeEnabled = false;
};

UCLASS()
class BULLETRUSH_API UMusicManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMusicManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void PlayOnChannel(FName ChannelName, USoundBase* Song, float StartTime = 0.0f, float FadeInTime = 0.0f, bool bEnableLoopFade = false);
	void StopChannel(FName ChannelName, float FadeOutTime = 0.5f);
	void FadeChannelVolume(FName ChannelName, float FadeDuration, float TargetVolume);
	float GetChannelPosition(FName ChannelName) const;
	bool IsChannelPlaying(FName ChannelName) const;

	void PlaySong(USoundBase* Song, float StartTime, float FadeInTime = 0.0f, bool bEnableLoopFade = false);
	void Stop(float FadeOutTime = 0.5f);
	void TransitionTo(USoundBase* NewSong, float FadeOutDuration = 3.0f,
	                  float FadeInDuration = 0.5f, float StartTime = 0.0f, bool bEnableLoopFade = false);

	void NotifyLevelTravel();
	void SavePlaybackPosition();
	bool IsPositionSaved() const { return bPendingResume; }
	float ConsumeSavedPosition();

	bool IsPlaying() const;

private:
	int32 GetOrCreateChannel(FName ChannelName);
	UAudioComponent* GetChannelComponent(FName ChannelName) const;

	void StartLoopFadeTimer();
	void StopLoopFadeTimer();
	UFUNCTION()
	void OnLoopFadeTick();

	UPROPERTY()
	TArray<UAudioComponent*> ChannelComponents;

	TMap<FName, int32> ChannelIndex;
	TMap<FName, FMusicChannelState> ChannelState;

	float SavedPlaybackPosition = 0.0f;
	bool bPendingResume = false;

	FTimerHandle GlobalLoopFadeTimerHandle;
};
