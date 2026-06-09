#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;
class UImage;
class USizeBox;
class UTextBlock;
class UCanvasPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseMenuEvent);

UCLASS()
class BULLETRUSH_API UPauseMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnPauseMenuEvent OnVolverAlMapa;

    UPROPERTY(BlueprintAssignable)
    FOnPauseMenuEvent OnContinuar;

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    void CreateWidgetTree();
    UButton* CreateMenuButton(const FString& Text, bool bIsVolverAlMapa, USizeBox*& OutSizeBox, UTextBlock*& OutLabel);

    UFUNCTION() void OnVolverClicked();
    UFUNCTION() void OnContinuarClicked();

    // Referencias internas para el caos visual en el Tick
    UPROPERTY() UImage* OverlayBgRef;
    UPROPERTY() UButton* VolverBtnRef;
    UPROPERTY() USizeBox* VolverSizeBoxRef;
    UPROPERTY() UTextBlock* VolverLabelRef;

    UPROPERTY() UButton* ContinuarBtnRef;
    UPROPERTY() USizeBox* ContinuarSizeBoxRef;
    UPROPERTY() UTextBlock* ContinuarLabelRef;

    // Controladores de interpolación fluida
    float VolverScaleTarget;
    float VolverCurrentScale;
    float ContinuarScaleTarget;
    float ContinuarCurrentScale;

    UPROPERTY() UTexture2D* FondoTex;
    UPROPERTY() UTexture2D* BotonTex;
};