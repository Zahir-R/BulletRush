#include "UI/EnemyHealthBarWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/CoreStyle.h"

void UEnemyHealthBarWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WidgetTree) return;

	UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootPanel"));
	WidgetTree->RootWidget = RootPanel;

	FrameBorder = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("FrameBorder"));
	FrameBorder->SetVisibility(ESlateVisibility::Hidden);
	FrameBorder->SetColorAndOpacity(FLinearColor(0.6f, 0.05f, 0.05f, 0.85f));
	RootPanel->AddChild(FrameBorder);
	if (UCanvasPanelSlot* FrameSlot = Cast<UCanvasPanelSlot>(FrameBorder->Slot))
	{
		FrameSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		FrameSlot->SetOffsets(FMargin(0.f));
	}

	BossNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("BossNameText"));
	BossNameText->SetVisibility(ESlateVisibility::Hidden);
	BossNameText->SetText(FText::FromString(TEXT("BOSS")));
	BossNameText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.85f, 0.2f, 1.f)));
	BossNameText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 14));
	BossNameText->SetJustification(ETextJustify::Center);
	RootPanel->AddChild(BossNameText);
	if (UCanvasPanelSlot* NameSlot = Cast<UCanvasPanelSlot>(BossNameText->Slot))
	{
		NameSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 0.35f));
		NameSlot->SetOffsets(FMargin(4.f, 0.f, 4.f, 0.f));
	}

	HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
	RootPanel->AddChild(HealthBar);
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HealthBar->Slot))
	{
		CanvasSlot->SetAnchors(FAnchors(0.f, 0.35f, 1.f, 1.f));
		CanvasSlot->SetOffsets(FMargin(2.f, 2.f, 2.f, 2.f));
	}

	HealthBar->SetPercent(1.0f);
	HealthBar->SetFillColorAndOpacity(HealthBarColor);
}

void UEnemyHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsBossWidget) return;
	if (!HealthBar) return;

	PulseTime += InDeltaTime;

	if (CachedHealthPercent < 0.3f)
	{
		float Pulse = 0.5f + 0.5f * FMath::Sin(PulseTime * 6.f);
		FLinearColor PulsedColor = FMath::Lerp(HealthBarColor, HealthBarColorLow, Pulse);
		HealthBar->SetFillColorAndOpacity(PulsedColor);
	}
	else
	{
		HealthBar->SetFillColorAndOpacity(HealthBarColor);
	}
}

void UEnemyHealthBarWidget::SetHealthPercent(float Percent)
{
	CachedHealthPercent = FMath::Clamp(Percent, 0.f, 1.f);
	if (HealthBar)
	{
		HealthBar->SetPercent(CachedHealthPercent);
	}
}

void UEnemyHealthBarWidget::SetIsBoss(bool bInIsBoss)
{
	bIsBossWidget = bInIsBoss;
	if (BossNameText)
	{
		BossNameText->SetVisibility(bInIsBoss ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
	if (FrameBorder)
	{
		FrameBorder->SetVisibility(bInIsBoss ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UEnemyHealthBarWidget::SetBossName(const FString& InName)
{
	BossName = InName;
	if (BossNameText)
	{
		BossNameText->SetText(FText::FromString(InName));
	}
}

void UEnemyHealthBarWidget::SetHealthBarColor(FLinearColor InColor)
{
	HealthBarColor = InColor;
	if (HealthBar)
	{
		HealthBar->SetFillColorAndOpacity(InColor);
	}
}

void UEnemyHealthBarWidget::SetHealthBarColorLow(FLinearColor InColor)
{
	HealthBarColorLow = InColor;
}
