#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RhytmConductorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeatHitSignature, bool, bIsStrongBeat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSilenceEnterSignature);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API URhytmConductorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URhytmConductorComponent();

	FOnBeatHitSignature OnBeatHit;
	FOnSilenceEnterSignature OnSilenceEnter;

	float BPM;
	
	void StartMusic();

	void EnterSilence();

protected:
	virtual void BeginPlay() override;
		
private:
	FTimerHandle RhytmTimerHandle;
	bool bToggleStrongBeat;

	void TriggerBeat();
};
