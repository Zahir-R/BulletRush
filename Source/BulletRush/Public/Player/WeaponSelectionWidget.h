#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WeaponSelectionWidget.generated.h"

class UButton;

UCLASS()
class BULLETRUSH_API UWeaponSelectionWidget
    : public UUserWidget
{
    GENERATED_BODY()

    UPROPERTY()
    UUserWidget* TestWidget;
protected:

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* AutoButton;

    UPROPERTY(meta = (BindWidget))
    UButton* PlusButton;

    UPROPERTY(meta = (BindWidget))
    UButton* VolleyButton;

    UFUNCTION()
    void OnAutoClicked();

    UFUNCTION()
    void OnPlusClicked();

    UFUNCTION()
    void OnVolleyClicked();
};