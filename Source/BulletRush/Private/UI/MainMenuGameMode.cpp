
#include "UI/MainMenuGameMode.h"
#include "UI/MainMenuWidget.h"

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    UMainMenuWidget* Widget = CreateWidget<UMainMenuWidget>(GetWorld(), UMainMenuWidget::StaticClass());
    if (Widget)
    {
        Widget->AddToViewport();
    }

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetShowMouseCursor(true);
        PC->SetInputMode(FInputModeUIOnly());
    }
}