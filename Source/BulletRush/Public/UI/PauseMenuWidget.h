#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

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

private:
    void CreateWidgetTree();
    UButton* CreateMenuButton(const FString& Text, bool bIsVolverAlMapa);
    UFUNCTION() void OnVolverClicked();
    UFUNCTION() void OnContinuarClicked();

    UPROPERTY()
    UTexture2D* FondoTex;

    UPROPERTY()
    UTexture2D* BotonTex;
};