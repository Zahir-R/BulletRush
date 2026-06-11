#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UCanvasPanel;
class UImage;
class UVerticalBox;
class UButton;
class UTexture2D;
class USizeBox;
class UTextBlock;

UCLASS()
class BULLETRUSH_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION()
    void OnJugarClicked();

    UFUNCTION()
    void OnSalirClicked();

private:
    void CreateBackground(UCanvasPanel* Root);
    void CreateLogo(UCanvasPanel* Root);
    void CreateButtons(UCanvasPanel* Root);
    UButton* CreateMenuButton(const FString& Text, UTexture2D* Icon, bool bFilled, USizeBox*& OutSizeBox, UTextBlock*& OutLabel);

    // Referencias guardadas para animar en el Tick
    UPROPERTY()
    UImage* LogoImageRef;

    UPROPERTY()
    UButton* JugarButtonRef;
    UPROPERTY()
    USizeBox* JugarSizeBoxRef;
    UPROPERTY()
    UTextBlock* JugarLabelRef;

    UPROPERTY()
    UButton* SalirButtonRef;
    UPROPERTY()
    USizeBox* SalirSizeBoxRef;
    UPROPERTY()
    UTextBlock* SalirLabelRef;

    // Estados de animación (Interpolación suave)
    float JugarHoverTarget;
    float JugarCurrentScale;
    float SalirHoverTarget;
    float SalirCurrentScale;

    // Assets
    UPROPERTY()
    UTexture2D* HexPatternTex;
    UPROPERTY()
    UTexture2D* LogoTex;
    UPROPERTY()
    UTexture2D* ButtonOutlineTex;
    UPROPERTY()
    UTexture2D* IconPlayTex;
    UPROPERTY()
    UTexture2D* IconExitTex;
};