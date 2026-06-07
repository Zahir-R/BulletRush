#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "EnemyHealthBarWidget.generated.h"

UCLASS()
class BULLETRUSH_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SetHealthPercent(float Percent);

	void SetIsBoss(bool bInIsBoss);
	void SetBossName(const FString& InName);
	void SetHealthBarColor(FLinearColor InColor);
	void SetHealthBarColorLow(FLinearColor InColor);

protected:
	UPROPERTY()
	UProgressBar* HealthBar;

	UPROPERTY()
	UTextBlock* BossNameText;

	UPROPERTY()
	UImage* FrameBorder;

	bool bIsBossWidget = false;
	FString BossName;
	FLinearColor HealthBarColor = FLinearColor::Red;
	FLinearColor HealthBarColorLow = FLinearColor::Red;
	float CachedHealthPercent = 1.f;
	float PulseTime = 0.f;
};
