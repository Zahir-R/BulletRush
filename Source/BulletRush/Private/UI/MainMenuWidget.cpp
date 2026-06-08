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
}

void UMainMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree) return;

    UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), NAME_None);
    WidgetTree->RootWidget = Root;

    // Matamos el cartel de iluminación en el frame 1 del menú
    if (UWorld* World = GetWorld())
    {
        UKismetSystemLibrary::ExecuteConsoleCommand(World, TEXT("DisableAllScreenMessages"));
    }

    CreateBackground(Root);
    CreateLogo(Root);
    CreateButtons(Root);
}

void UMainMenuWidget::CreateBackground(UCanvasPanel* Root)
{
    UImage* Bg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);

    if (HexPatternTex)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(HexPatternTex);
        // NO usamos Tiling para que el fondo con brillos ocupe toda la pantalla de una pieza
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

    UImage* Logo = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);

    FSlateBrush Brush;
    Brush.SetResourceObject(LogoTex);
    // Tamaño masivo y cinemático para el título del juego
    Brush.ImageSize = FVector2D(1000.0f, 400.0f);
    Logo->SetBrush(Brush);

    Root->AddChild(Logo);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Logo->Slot))
    {
        // Bajamos el anclaje a 0.35 para que llene el espacio central superior
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.35f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
        CanvasSlot->SetSize(FVector2D(2000.0f, 800.0f));
    }
}

void UMainMenuWidget::CreateButtons(UCanvasPanel* Root)
{
    UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), NAME_None);

    UButton* JugarBtn = CreateMenuButton(TEXT("JUGAR"), IconPlayTex, true);
    UButton* SalirBtn = CreateMenuButton(TEXT("SALIR"), IconExitTex, false);

    Box->AddChild(JugarBtn);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(JugarBtn->Slot))
    {
        // 28 píxeles de espacio libre entre ellos para que respiren bien
        VBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 28.0f));
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    Box->AddChild(SalirBtn);
    if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(SalirBtn->Slot))
    {
        VBoxSlot->SetHorizontalAlignment(HAlign_Center);
    }

    JugarBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJugarClicked);
    SalirBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSalirClicked);

    Root->AddChild(Box);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Box->Slot))
    {
        // Subimos ligeramente el bloque de botones para que ensamble pegado al logo
        CanvasSlot->SetAnchors(FAnchors(0.5f, 0.72f));
        CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
        CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));
        CanvasSlot->SetAutoSize(true);
    }
}

UButton* UMainMenuWidget::CreateMenuButton(const FString& Text, UTexture2D* Icon, bool bFilled)
{
    // Dimensiones robustas y Premium para pantalla completa
    const float BtnW = 540.0f;
    const float BtnH = 110.0f;

    UButton* Btn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), NAME_None);
    FButtonStyle Style = Btn->WidgetStyle;

    auto SetupBrush = [&](FSlateBrush& Brush, const FLinearColor& Tint)
        {
            if (ButtonOutlineTex)
            {
                Brush.SetResourceObject(ButtonOutlineTex);
                Brush.ImageSize = FVector2D(BtnW, BtnH);
                Brush.DrawAs = ESlateBrushDrawType::Image;
            }
            Brush.TintColor = FSlateColor(Tint);
        };

    FLinearColor CianBrillante = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);
    FLinearColor CianHover = FLinearColor(0.2f, 0.7f, 1.0f, 1.0f);

    SetupBrush(Style.Normal, CianBrillante);
    SetupBrush(Style.Hovered, CianHover);
    SetupBrush(Style.Pressed, CianBrillante * 0.6f);
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
        EmptyFill->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.3f));
        Overlay->AddChildToOverlay(EmptyFill);
    }

    UHorizontalBox* HBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), NAME_None);

    if (Icon)
    {
        UImage* IconImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), NAME_None);
        FSlateBrush IconBrush;
        IconBrush.SetResourceObject(Icon);
        // Icono más grande proporcional al nuevo botón
        IconBrush.ImageSize = FVector2D(36.0f, 36.0f);
        IconImg->SetBrush(IconBrush);

        HBox->AddChild(IconImg);
        if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(IconImg->Slot))
        {
            HBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
            HBoxSlot->SetVerticalAlignment(VAlign_Center);
        }
    }

    UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), NAME_None);
    Label->SetText(FText::FromString(Text));
    // Fuente más grande para rellenar el botón con autoridad
    Label->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 26));

    HBox->AddChild(Label);
    if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(Label->Slot))
    {
        HBoxSlot->SetVerticalAlignment(VAlign_Center);
    }

    Overlay->AddChildToOverlay(HBox);
    if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(HBox->Slot))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Center);
        OverlaySlot->SetVerticalAlignment(VAlign_Center);
    }

    USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), NAME_None);
    SizeBox->SetWidthOverride(BtnW);
    SizeBox->SetHeightOverride(BtnH);
    SizeBox->SetContent(Overlay);
    Btn->SetContent(SizeBox);

    return Btn;
}

void UMainMenuWidget::OnJugarClicked()
{
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