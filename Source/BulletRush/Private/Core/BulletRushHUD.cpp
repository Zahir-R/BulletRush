#include "Core/BulletRushHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Engine/Engine.h"

void ABulletRushHUD::DrawHUD()
{
	Super::DrawHUD();

	UFont* RenderFont = GEngine->GetMediumFont();
	if (!RenderFont || !Canvas) return;

	float X = Canvas->SizeX * 0.5f;
	float Y = 50.f;

	if (bShowCountdown && CountdownTime >= 0.f)
	{
		FString CountdownStr = FString::Printf(TEXT("Tiempo restante: %.0f"), CountdownTime);
		float TextX = X - (RenderFont->GetStringSize(*CountdownStr) * 0.5f);
		DrawText(CountdownStr, FLinearColor::White, TextX, Y, RenderFont);
		Y += 40.f;
	}

	if (!CurrentMessage.IsEmpty())
	{
		float MsgX = X - (RenderFont->GetStringSize(*CurrentMessage) * 0.5f);
		DrawText(CurrentMessage, FLinearColor::Red, MsgX, Y, RenderFont);
	}
}

void ABulletRushHUD::SetCountdown(float TimeLeft)
{
	CountdownTime = TimeLeft;
	bShowCountdown = (TimeLeft >= 0.f);
}

void ABulletRushHUD::ShowMessage(const FString& Msg, float Duration)
{
	CurrentMessage = Msg;
	GetWorldTimerManager().ClearTimer(MessageTimerHandle);
	if (Duration > 0.f)
	{
		GetWorldTimerManager().SetTimer(MessageTimerHandle, this, &ABulletRushHUD::ClearMessage, Duration, false);
	}
}

void ABulletRushHUD::ClearMessage()
{
	CurrentMessage.Empty();
}
