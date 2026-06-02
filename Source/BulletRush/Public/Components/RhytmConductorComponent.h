#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RhytmConductorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeatSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSilenceSignature, bool, bIsSilence);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API URhytmConductorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URhytmConductorComponent();

	UPROPERTY(BlueprintAssignable, Category = "Music Events")
	FOnBeatSignature OnBeat;

	UPROPERTY(BlueprintAssignable, Category = "Music Events")
	FOnSilenceSignature OnSilence;

	float BPM;
	
	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void StartRhythm(float InBPM);

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void StopRhythm();

	UFUNCTION(BlueprintCallable, Category = "Rhythm")
	void TriggerSilence(bool bActivateSilence);

protected:
	void BroadcastBeat();
		
private:
	FTimerHandle RhytmTimerHandle;
	bool bToggleStrongBeat;

	//void TriggerBeat();
};
