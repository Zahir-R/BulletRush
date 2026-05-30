#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BulletRushHUD.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API ABulletRushHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	void SetCountdown(float TimeLeft);
	void ShowMessage(const FString& Msg, float Duration);

protected:
	float CountdownTime = -1.f;
	bool bShowCountdown = false;
	FString CurrentMessage;
	float MessageTimeRemaining = 0.f;
	FTimerHandle MessageTimerHandle;

	void ClearMessage();
};
