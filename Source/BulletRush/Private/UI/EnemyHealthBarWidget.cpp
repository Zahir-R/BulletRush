#include "UI/EnemyHealthBarWidget.h"
#include "Blueprint/WidgetTree.h"

void UEnemyHealthBarWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree) return;

    UCanvasPanel* RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootPanel"));
    WidgetTree->RootWidget = RootPanel;

    HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("HealthBar"));
    RootPanel->AddChild(HealthBar);

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(HealthBar->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        CanvasSlot->SetOffsets(FMargin(0.0f));
    }

    HealthBar->SetPercent(1.0f);
    HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
}

void UEnemyHealthBarWidget::SetHealthPercent(float Percent)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
    }
}