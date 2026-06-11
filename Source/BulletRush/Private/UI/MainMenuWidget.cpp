#include "UI/MainMenuWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Styling/CoreStyle.h"

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    HexPatternTex = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Menu/fondo.fondo'"));
    LogoTex = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Menu/logo.logo'"));
    ButtonOutlineTex = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Menu/boton.boton'"));
    IconPlayTex = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Menu/boton.boton'"));
    IconExitTex = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Menu/boton.boton'"));

    JugarHoverTarget = 1.0f;
    JugarCurrentScale = 1.0f;
    SalirHoverTarget = 1.0f;
    SalirCurrentScale = 1.0f;
}

void UMainMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree) return;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), NAME_None);
    WidgetTree->RootWidget = Root;

    if (UWorld* World = GetWorld())
    {
        UKismetSystemLibrary::ExecuteConsoleCommand(World, TEXT("DisableAllScreenMessages"));
    }

    CreateBackground(Root);
    CreateLogo(Root);
    CreateButtons(Root);
}

void UMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 1. EFECTO GLOW/FLICKER EN EL LOGO (Estilo Neo-Cian)
    if (LogoImageRef)
    {
        float Time = GetWorld() ? GetWorld()->GetRealTimeSeconds() : 0.0f;
        // Mezcla de ondas seno para simular un parpadeo de energía cibernética realista
        float Glow = 0.75f + (FMath::Sin(Time * 4.0f) * 0.15f) + (FMath::Sin(Time * 25.0f) * 0.05f);
        LogoImageRef->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, Glow));
    }

    // 2. INTERPOLACIÓN SUAVE PARA EL BOTÓN JUGAR
    if (JugarButtonRef && JugarSizeBoxRef)
    {
        JugarHoverTarget = JugarButtonRef->IsHovered() ? 1.08f : 1.0f;
        JugarCurrentScale = FMath::FInterpTo(JugarCurrentScale, JugarHoverTarget, InDeltaTime, 12.0f);

        JugarSizeBoxRef->SetWidthOverride(540.0f * JugarCurrentScale);
        JugarSizeBoxRef->SetHeightOverride(110.0f * JugarCurrentScale);

        if (JugarLabelRef)
        {
            FLinearColor TextColor = JugarButtonRef->IsHovered() ? FLinearColor(1.0f, 1.0f, 1.0f, 1.0f) : FLinearColor(0.7f, 0.9f, 1.0f, 0.9f);
            JugarLabelRef->SetColorAndOpacity(FSlateColor(TextColor));
        }
    }

    // 3. INTERPOLACIÓN SUAVE PARA EL BOTÓN SALIR
    if (SalirButtonRef && SalirSizeBoxRef)
    {
        SalirHoverTarget = SalirButtonRef->IsHovered() ? 1.08f : 1.0f;
        SalirCurrentScale = FMath::FInterpTo(SalirCurrentScale, SalirHoverTarget, InDeltaTime, 12.0f);

        SalirSizeBoxRef->SetWidthOverride(540.0f * SalirCurrentScale);
        SalirSizeBoxRef->SetHeightOverride(110.0f * SalirCurrentScale);

        if (SalirLabelRef)
        {
            FLinearColor TextColor = SalirButtonRef->IsHovered() ? FLinearColor(0.2f, 0.8f, 1.0f, 1.0f) : FLinearColor(0.0f, 0.5f, 1.0f, 0.75f);
            SalirLabelRef->SetColorAndOpacity(FSlateColor(TextColor));
        }
    }
}

void UMainMenuWidget::CreateBackground(UCanvasPanel* Root)
{
    UImage* Bg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);

    if (HexPatternTex)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(HexPatternTex);
        Brush.DrawAs = ESlateBrushDrawType::Image;
        Bg->SetBrush(Brush);
    }

    Root->AddChild(Bg);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Bg->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
        CanvasSlot->SetOffsets(FMargin(0.0f));
    }
}

void UMainMenuWidget::CreateLogo(UCanvasPanel* Root)
{
    if (!LogoTex) return;

    LogoImageRef = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);

    FSlateBrush Brush;
    Brush.SetResourceObject(LogoTex);
    Brush.ImageSize = FVector2D(10000.0f, 400.0f);
    LogoImageRef->SetBrush(Brush);

    Root->AddChild(LogoImageRef);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(LogoImageRef->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.35f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
        // FIX: Corregido el tamaño del slot para mapear 1:1 con el tamaño real de la brocha
        CanvasSlot->SetSize(FVector2D(1000.0f, 400.0f));
    }
}

void UMainMenuWidget::CreateButtons(UCanvasPanel* Root)
{
    UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), NAME_None);

    JugarButtonRef = CreateMenuButton(TEXT("JUGAR"), IconPlayTex, true, JugarSizeBoxRef, JugarLabelRef);
    SalirButtonRef = CreateMenuButton(TEXT("SALIR"), IconExitTex, false, SalirSizeBoxRef, SalirLabelRef);

    Box->AddChild(JugarButtonRef);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(JugarButtonRef->Slot))
    {
        VBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 32.0f)); // Incrementado ligeramente el aire entre componentes
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    Box->AddChild(SalirButtonRef);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(SalirButtonRef->Slot))
    {
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    JugarButtonRef->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJugarClicked);
    SalirButtonRef->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSalirClicked);

    Root->AddChild(Box);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Box->Slot))
    {
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.74f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
        CanvasSlot->SetAutoSize(true);
    }
}

UButton* UMainMenuWidget::CreateMenuButton(const FString& Text, UTexture2D* Icon, bool bFilled, USizeBox*& OutSizeBox, UTextBlock*& OutLabel)
{
    const float DefaultW = 540.0f;
    const float DefaultH = 110.0f;

    UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), NAME_None);
    FButtonStyle Style = Btn->WidgetStyle;

    auto SetupBrush = [&](FSlateBrush& Brush, const FLinearColor& Tint)
        {
            if (ButtonOutlineTex)
            {
                Brush.SetResourceObject(ButtonOutlineTex);
                Brush.ImageSize = FVector2D(DefaultW, DefaultH);
                Brush.DrawAs = ESlateBrushDrawType::Image;
            }
            Brush.TintColor = FSlateColor(Tint);
        };

    FLinearColor CianBrillante = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);
    FLinearColor CianHover = FLinearColor(0.3f, 0.8f, 1.0f, 1.0f);

    SetupBrush(Style.Normal, CianBrillante);
    SetupBrush(Style.Hovered, CianHover);
    SetupBrush(Style.Pressed, CianBrillante * 0.5f);

    // Eliminamos bordes y líneas nativas molestas de Slate al enfocar con teclado/control
    Style.Disabled.DrawAs = ESlateBrushDrawType::NoDrawType;
    Btn->SetStyle(Style);

    UOverlay* Overlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), NAME_None);

    if (bFilled)
    {
        UImage* Fill = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);
        Fill->SetColorAndOpacity(FLinearColor(0.0f, 0.35f, 0.85f, 0.75f));
        Overlay->AddChildToOverlay(Fill);
    }
    else
    {
        UImage* EmptyFill = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);
        EmptyFill->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.4f));
        Overlay->AddChildToOverlay(EmptyFill);
    }

    UHorizontalBox* HBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), NAME_None);

    if (Icon)
    {
        UImage* IconImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);
        FSlateBrush IconBrush;
        IconBrush.SetResourceObject(Icon);
        IconBrush.ImageSize = FVector2D(36.0f, 36.0f);
        IconImg->SetBrush(IconBrush);

        HBox->AddChild(IconImg);
        if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(IconImg->Slot))
        {
            HBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 18.0f, 0.0f));
            HBoxSlot->SetVerticalAlignment(VAlign_Center);
        }
    }

    OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), NAME_None);
    OutLabel->SetText(FText::FromString(Text));
    OutLabel->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 26));

    HBox->AddChild(OutLabel);
    if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(OutLabel->Slot))
    {
        HBoxSlot->SetVerticalAlignment(VAlign_Center);
    }

    Overlay->AddChildToOverlay(HBox);
    if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(HBox->Slot))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Center);
        OverlaySlot->SetVerticalAlignment(VAlign_Center);
    }

    OutSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), NAME_None);
    OutSizeBox->SetWidthOverride(DefaultW);
    OutSizeBox->SetHeightOverride(DefaultH);
    OutSizeBox->SetContent(Overlay);
    Btn->SetContent(OutSizeBox);

    return Btn;
}

void UMainMenuWidget::OnJugarClicked()
{
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
    }
    UGameplayStatics::OpenLevel(this, TEXT("Map_CupHeadMap"));
}

void UMainMenuWidget::OnSalirClicked()
{
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
    }
}