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
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"

void UPauseMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (!WidgetTree) return;

    FondoTex = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Menu/fondo.fondo'"));
    BotonTex = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Menu/boton.boton'"));

    // Inicialización de escalas base
    VolverScaleTarget = 1.0f;
    VolverCurrentScale = 1.0f;
    ContinuarScaleTarget = 1.0f;
    ContinuarCurrentScale = 1.0f;

    CreateWidgetTree();
}

void UPauseMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    float TimeSeconds = GetWorld() ? GetWorld()->GetRealTimeSeconds() : 0.0f;

    // 1. GLITCH DE FONDO CIBERNÉTICO (Parpadeo agresivo de opacidad)
    if (OverlayBgRef)
    {
        // Interferencia basada en frecuencias cruzadas de ondas senoidales
        float BasePulse = FMath::Sin(TimeSeconds * 8.0f) * 0.08f;
        float MicroGlitch = (FMath::Sin(TimeSeconds * 45.0f) * FMath::Cos(TimeSeconds * 30.0f)) * 0.04f;
        float FinalOpacity = 0.75f + BasePulse + MicroGlitch;

        // El fondo parpadea sutilmente en tonos rojos/oscuros de emergencia
        OverlayBgRef->SetColorAndOpacity(FLinearColor(0.05f, 0.0f, 0.0f, FMath::Clamp(FinalOpacity, 0.5f, 0.95f)));
    }

    // 2. ANIMACIÓN + EFECTO DE SACUDIDA ELECTROMAGNÉTICA (VOLVER AL MAPA)
    if (VolverBtnRef && VolverSizeBoxRef)
    {
        bool bIsHovered = VolverBtnRef->IsHovered();
        VolverScaleTarget = bIsHovered ? 1.12f : 1.0f; // Escalado exagerado
        VolverCurrentScale = FMath::FInterpTo(VolverCurrentScale, VolverScaleTarget, InDeltaTime, 14.0f);

        // Si el jugador pasa el mouse, el botón vibra violentamente por el glitch
        float ShakeX = bIsHovered ? (FMath::RandRange(-4.0f, 4.0f)) : 0.0f;
        float ShakeY = bIsHovered ? (FMath::RandRange(-2.0f, 2.0f)) : 0.0f;

        VolverSizeBoxRef->SetWidthOverride((520.0f * VolverCurrentScale) + ShakeX);
        VolverSizeBoxRef->SetHeightOverride((105.0f * VolverCurrentScale) + ShakeY);

        if (VolverLabelRef)
        {
            // El texto parpadea entre blanco y rojo neón al enfocarlo
            FLinearColor FocusedColor = (FMath::Sin(TimeSeconds * 35.0f) > 0.0f) ? FLinearColor(1.0f, 1.0f, 1.0f, 1.0f) : FLinearColor(1.0f, 0.2f, 0.2f, 1.0f);
            VolverLabelRef->SetColorAndOpacity(FSlateColor(bIsHovered ? FocusedColor : FLinearColor(0.8f, 0.3f, 0.3f, 0.85f)));
        }
    }

    // 3. ANIMACIÓN + EFECTO DE SACUDIDA ELECTROMAGNÉTICA (CONTINUAR)
    if (ContinuarBtnRef && ContinuarSizeBoxRef)
    {
        bool bIsHovered = ContinuarBtnRef->IsHovered();
        ContinuarScaleTarget = bIsHovered ? 1.12f : 1.0f;
        ContinuarCurrentScale = FMath::FInterpTo(ContinuarCurrentScale, ContinuarScaleTarget, InDeltaTime, 14.0f);

        float ShakeX = bIsHovered ? (FMath::RandRange(-4.0f, 4.0f)) : 0.0f;
        float ShakeY = bIsHovered ? (FMath::RandRange(-2.0f, 2.0f)) : 0.0f;

        ContinuarSizeBoxRef->SetWidthOverride((520.0f * ContinuarCurrentScale) + ShakeX);
        ContinuarSizeBoxRef->SetHeightOverride((105.0f * ContinuarCurrentScale) + ShakeY);

        if (ContinuarLabelRef)
        {
            // El texto parpadea entre blanco y cian al enfocarlo
            FLinearColor FocusedColor = (FMath::Sin(TimeSeconds * 35.0f) > 0.0f) ? FLinearColor(1.0f, 1.0f, 1.0f, 1.0f) : FLinearColor(0.0f, 0.7f, 1.0f, 1.0f);
            ContinuarLabelRef->SetColorAndOpacity(FSlateColor(bIsHovered ? FocusedColor : FLinearColor(0.2f, 0.5f, 0.9f, 0.85f)));
        }
    }
}

void UPauseMenuWidget::CreateWidgetTree()
{
    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
    WidgetTree->RootWidget = Root;

    OverlayBgRef = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("OverlayBg"));
    if (FondoTex)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(FondoTex);
        Brush.DrawAs = ESlateBrushDrawType::Image;
        OverlayBgRef->SetBrush(Brush);
    }
    else
    {
        OverlayBgRef->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.75f));
    }

    Root->AddChild(OverlayBgRef);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(OverlayBgRef->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        CanvasSlot->SetOffsets(FMargin(0.0f));
    }

    UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ButtonBox"));

    VolverBtnRef = CreateMenuButton(TEXT("VOLVER AL MAPA"), true, VolverSizeBoxRef, VolverLabelRef);
    ContinuarBtnRef = CreateMenuButton(TEXT("CONTINUAR"), false, ContinuarSizeBoxRef, ContinuarLabelRef);

    VolverBtnRef->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnVolverClicked);
    ContinuarBtnRef->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnContinuarClicked);

    Box->AddChild(VolverBtnRef);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(VolverBtnRef->Slot))
    {
        VBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 24.0f)); // Mayor espacio de separación estética
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    Box->AddChild(ContinuarBtnRef);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(ContinuarBtnRef->Slot))
    {
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    Root->AddChild(Box);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Box->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
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

UButton* UPauseMenuWidget::CreateMenuButton(const FString& Text, bool bIsVolverAlMapa, USizeBox*& OutSizeBox, UTextBlock*& OutLabel)
{
    const float BtnW = 520.0f;
    const float BtnH = 105.0f;

    UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
    FButtonStyle Style = Btn->WidgetStyle;

    // Alertas de tonalidad agresiva: Rojo sangre para Volver, Azul Eléctrico para Continuar
    FLinearColor BaseColor = bIsVolverAlMapa ? FLinearColor(0.85f, 0.1f, 0.1f, 1.0f) : FLinearColor(0.0f, 0.4f, 0.9f, 1.0f);
    FLinearColor HoverColor = bIsVolverAlMapa ? FLinearColor(1.0f, 0.3f, 0.3f, 1.0f) : FLinearColor(0.2f, 0.6f, 1.0f, 1.0f);
    FLinearColor PressedColor = BaseColor * 0.4f;

    auto SetupBrush = [&](FSlateBrush& Brush, const FLinearColor& Tint)
        {
            if (BotonTex)
            {
                Brush.SetResourceObject(BotonTex);
                Brush.DrawAs = ESlateBrushDrawType::Image;
            }
            else
            {
                Brush.DrawAs = ESlateBrushDrawType::Box;
                Brush.Margin = FMargin(14.0f);
            }
            Brush.ImageSize = FVector2D(BtnW, BtnH);
            Brush.TintColor = FSlateColor(Tint);
        };

    SetupBrush(Style.Normal, BaseColor);
    SetupBrush(Style.Hovered, HoverColor);
    SetupBrush(Style.Pressed, PressedColor);
    Style.Disabled.DrawAs = ESlateBrushDrawType::NoDrawType;
    Btn->SetStyle(Style);

    UOverlay* Overlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());

    // Añadimos un panel de fondo oscuro interno para darle contraste al texto del botón
    UImage* InlineFill = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    InlineFill->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.35f));
    Overlay->AddChildToOverlay(InlineFill);

    UHorizontalBox* HBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

    OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    OutLabel->SetText(FText::FromString(Text));
    OutLabel->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 24));
    OutLabel->SetJustification(ETextJustify::Center);

    HBox->AddChild(OutLabel);
    if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(OutLabel->Slot))
    {
        HBoxSlot->SetHorizontalAlignment(HAlign_Center);
        HBoxSlot->SetVerticalAlignment(VAlign_Center);
    }

    Overlay->AddChildToOverlay(HBox);
    if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(HBox->Slot))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Center);
        OverlaySlot->SetVerticalAlignment(VAlign_Center);
    }

    OutSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
    OutSizeBox->SetWidthOverride(BtnW);
    OutSizeBox->SetHeightOverride(BtnH);
    OutSizeBox->SetContent(Overlay);
    Btn->SetContent(OutSizeBox);

    return Btn;
}