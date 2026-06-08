#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UCanvasPanel;
class UImage;
class UVerticalBox;
class UButton;
class UTexture2D;

UCLASS()
class BULLETRUSH_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;

    UFUNCTION()
    void OnJugarClicked();

    UFUNCTION()
    void OnSalirClicked();

private:
    void CreateBackground(UCanvasPanel* Root);
    void CreateLogo(UCanvasPanel* Root);
    void CreateButtons(UCanvasPanel* Root);
    UButton* CreateMenuButton(const FString& Text, UTexture2D* Icon, bool bFilled);

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