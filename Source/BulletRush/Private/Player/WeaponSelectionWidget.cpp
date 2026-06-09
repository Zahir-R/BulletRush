#include "Player/WeaponSelectionWidget.h"
#include "BulletRush/Public/Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"


void UWeaponSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (AutoButton)
    {
        AutoButton->OnClicked.AddDynamic(
            this,
            &UWeaponSelectionWidget::OnAutoClicked
        );
    }

    if (PlusButton)
    {
        PlusButton->OnClicked.AddDynamic(
            this,
            &UWeaponSelectionWidget::OnPlusClicked
        );
    }

    if (VolleyButton)
    {
        VolleyButton->OnClicked.AddDynamic(
            this,
            &UWeaponSelectionWidget::OnVolleyClicked
        );
    }
}

void UWeaponSelectionWidget::OnAutoClicked()
{
    APlayingPlayer* Player =
        Cast<APlayingPlayer>(
            UGameplayStatics::GetPlayerPawn(
                GetWorld(),
                0
            )
        );

    if (Player)
    {
        Player->SelectWeapon(0);
    }
}

void UWeaponSelectionWidget::OnPlusClicked()
{
    APlayingPlayer* Player =
        Cast<APlayingPlayer>(
            UGameplayStatics::GetPlayerPawn(
                GetWorld(),
                0
            )
        );

    if (Player)
    {
        Player->SelectWeapon(1);
    }
}

void UWeaponSelectionWidget::OnVolleyClicked()
{
    APlayingPlayer* Player =
        Cast<APlayingPlayer>(
            UGameplayStatics::GetPlayerPawn(
                GetWorld(),
                0
            )
        );

    if (Player)
    {
        Player->SelectWeapon(2);
    }
}