#include "UI/PauseMenuWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Styling/CoreStyle.h"

void UPauseMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (!WidgetTree) return;

    FondoTex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Menu/fondo"));
    BotonTex = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Menu/boton"));

    CreateWidgetTree();
}

void UPauseMenuWidget::CreateWidgetTree()
{
    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
    WidgetTree->RootWidget = Root;

    UImage* OverlayBg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("OverlayBg"));
    if (FondoTex)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(FondoTex);
        Brush.ImageSize = FVector2D(1920.0f, 1080.0f);
        Brush.DrawAs = ESlateBrushDrawType::Image;
        OverlayBg->SetBrush(Brush);
    }
    else
    {
        OverlayBg->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.7f));
    }
    Root->AddChild(OverlayBg);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(OverlayBg->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        CanvasSlot->SetOffsets(FMargin(0.0f));
    }

    UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ButtonBox"));

    UButton* VolverBtn = CreateMenuButton(TEXT("VOLVER AL MAPA"), true);
    UButton* ContinuarBtn = CreateMenuButton(TEXT("CONTINUAR"), false);

    VolverBtn->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnVolverClicked);
    ContinuarBtn->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnContinuarClicked);

    Box->AddChild(VolverBtn);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(VolverBtn->Slot))
    {
        VBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    Box->AddChild(ContinuarBtn);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(ContinuarBtn->Slot))
    {
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    Root->AddChild(Box);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Box->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetAutoSize(true);
    }
}

void UPauseMenuWidget::OnVolverClicked()
{
    OnVolverAlMapa.Broadcast();
}

void UPauseMenuWidget::OnContinuarClicked()
{
    OnContinuar.Broadcast();
}

UButton* UPauseMenuWidget::CreateMenuButton(const FString& Text, bool bIsVolverAlMapa)
{
    const float BtnW = 500.0f;
    const float BtnH = 100.0f;

    UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());

    FButtonStyle Style = Btn->WidgetStyle;

    if (BotonTex)
    {
        auto SetupBrush = [&](FSlateBrush& Brush, const FLinearColor& Tint)
        {
            Brush.SetResourceObject(BotonTex);
            Brush.ImageSize = FVector2D(BtnW, BtnH);
            Brush.DrawAs = ESlateBrushDrawType::Image;
            Brush.TintColor = FSlateColor(Tint);
        };

        FLinearColor BaseColor = bIsVolverAlMapa ? FLinearColor(0.8f, 0.2f, 0.2f, 1.0f) : FLinearColor(0.1f, 0.35f, 0.85f, 1.0f);
        FLinearColor HoverColor = BaseColor;
        HoverColor.R = FMath::Min(HoverColor.R + 0.15f, 1.0f);
        HoverColor.G = FMath::Min(HoverColor.G + 0.15f, 1.0f);
        HoverColor.B = FMath::Min(HoverColor.B + 0.15f, 1.0f);
        FLinearColor PressedColor = BaseColor;
        PressedColor.R *= 0.7f;
        PressedColor.G *= 0.7f;
        PressedColor.B *= 0.7f;

        SetupBrush(Style.Normal, BaseColor);
        SetupBrush(Style.Hovered, HoverColor);
        SetupBrush(Style.Pressed, PressedColor);
    }
    else
    {
        FLinearColor BaseColor = bIsVolverAlMapa ? FLinearColor(0.8f, 0.2f, 0.2f, 0.9f) : FLinearColor(0.1f, 0.35f, 0.85f, 0.9f);
        FLinearColor HoverColor = BaseColor; HoverColor.R = FMath::Min(HoverColor.R + 0.15f, 1.0f); HoverColor.G = FMath::Min(HoverColor.G + 0.15f, 1.0f); HoverColor.B = FMath::Min(HoverColor.B + 0.15f, 1.0f);
        FLinearColor PressedColor = BaseColor; PressedColor.R *= 0.7f; PressedColor.G *= 0.7f; PressedColor.B *= 0.7f;

        auto SetBrush = [&](FSlateBrush& Brush, const FLinearColor& Tint)
        {
            Brush.TintColor = FSlateColor(Tint);
            Brush.DrawAs = ESlateBrushDrawType::Box;
            Brush.Margin = FMargin(12.0f);
        };

        SetBrush(Style.Normal, BaseColor);
        SetBrush(Style.Hovered, HoverColor);
        SetBrush(Style.Pressed, PressedColor);
    }

    Btn->SetStyle(Style);

    UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Label->SetText(FText::FromString(Text));
    Label->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 22));
    Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    Label->SetJustification(ETextJustify::Center);

    UHorizontalBox* HBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
    HBox->AddChild(Label);
    if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(Label->Slot))
    {
        HBoxSlot->SetHorizontalAlignment(HAlign_Center);
        HBoxSlot->SetVerticalAlignment(VAlign_Center);
    }

    Btn->SetContent(HBox);
    return Btn;
}