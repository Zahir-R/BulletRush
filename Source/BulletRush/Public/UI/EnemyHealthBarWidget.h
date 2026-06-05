#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "EnemyHealthBarWidget.generated.h"

UCLASS()
class BULLETRUSH_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void SetHealthPercent(float Percent);

protected:
	UPROPERTY()
	UProgressBar* HealthBar;
};