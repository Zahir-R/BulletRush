#include "Core/BulletRushHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Engine/Engine.h"
#include "RenderUtils.h"
#include "Player/PlayingPlayer.h"
#include "Components/HealthComponent.h"
#include "Components/BuffComponent.h"
#include "Components/WeaponBaseComponent.h"
#include "Components/Weapons/AutoFireStrategy.h"
#include "Components/Weapons/VolleyStrategy.h"
#include "Components/Weapons/PlusFireStrategy.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "Core/BulletRushGameInstance.h"
#include "Enemies/EnemyBase.h"
#include "Kismet/GameplayStatics.h"

void ABulletRushHUD::BeginPlay()
{
    Super::BeginPlay();
    BindToPlayer();
}

void ABulletRushHUD::EndPlay(const EEndPlayReason::Type Reason)
{
    if (CachedPlayer.IsValid())
    {
        if (UHealthComponent* HC = CachedPlayer->HealthComp)
        {
            HC->OnHealthChanged.RemoveDynamic(this, &ABulletRushHUD::OnPlayerHealthChanged);
            HC->OnDeath.RemoveDynamic(this, &ABulletRushHUD::OnPlayerDeath);
        }
    }
    Super::EndPlay(Reason);
}

void ABulletRushHUD::BindToPlayer()
{
    APlayerController* PC = GetOwningPlayerController();
    if (!PC) return;

    APlayingPlayer* P = Cast<APlayingPlayer>(PC->GetPawn());
    if (!P) return;

    CachedPlayer = P;
    if (UHealthComponent* HC = P->HealthComp)
    {
        CachedHealth = HC->CurrentHealth;
        CachedMaxHealth = HC->MaxHealth;
        HC->OnHealthChanged.AddDynamic(this, &ABulletRushHUD::OnPlayerHealthChanged);
        HC->OnDeath.AddDynamic(this, &ABulletRushHUD::OnPlayerDeath);
    }

    if (UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
    {
        if (UBulletRushGameInstance* BRGI = Cast<UBulletRushGameInstance>(GI))
        {
            FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
            CachedLives = BRGI->GetVidasRestantes(FName(*LevelName));
        }
    }

}

void ABulletRushHUD::OnPlayerHealthChanged(float NewHealth)
{
    if (NewHealth < CachedHealth)
    {
        DamageFlashTime = 0.25f;
    }
    CachedHealth = NewHealth;
    if (CachedPlayer.IsValid() && CachedPlayer->HealthComp)
        CachedMaxHealth = CachedPlayer->HealthComp->MaxHealth;
}

void ABulletRushHUD::OnPlayerDeath()
{
    CachedDeaths++;
    if (UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
    {
        if (UBulletRushGameInstance* BRGI = Cast<UBulletRushGameInstance>(GI))
        {
            FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
            CachedLives = BRGI->GetVidasRestantes(FName(*LevelName));
        }
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

void ABulletRushHUD::SetObjective(const FString& NewObjective)
{
    CachedObjective = NewObjective;
}

void ABulletRushHUD::DrawPanelBorder(float X, float Y, float W, float H, float GlowAlpha, float CornerRadius)
{
    // Fondo redondeado
    DrawRoundedRect(X, Y, W, H, CornerRadius, FLinearColor(0.02f, 0.05f, 0.1f, 0.7f));

    // Pulse
    float Pulse = (FMath::Sin(TimeAccumulator * 2.5f) + 1.f) * 0.5f;
    float CurrentGlowAlpha = FMath::Lerp(GlowAlpha * 0.4f, GlowAlpha, Pulse);

    // Glow externo (rect redondeado un poco más grande con alpha bajo)
    float GlowExpand = 4.f;
    FLinearColor Glow(0.f, 0.7f, 1.f, CurrentGlowAlpha);
    DrawRoundedRect(X - GlowExpand, Y - GlowExpand, W + 2 * GlowExpand, H + 2 * GlowExpand,
        CornerRadius + GlowExpand, Glow);

    // Borde interno redondeado
    FLinearColor Border(0.f, 1.f, 1.f, 0.9f);
    DrawRoundedRectBorder(X, Y, W, H, CornerRadius, 2.f, Border);
}

void ABulletRushHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!CachedPlayer.IsValid())
    {
        BindToPlayer();
    }
    if (GetWorld())
    {
        TimeAccumulator += GetWorld()->GetDeltaSeconds();
    }
    // Damage flash overlay
    if (DamageFlashTime > 0.f)
    {
        DamageFlashTime -= GetWorld()->GetDeltaSeconds();
        float FlashAlpha = FMath::Clamp(DamageFlashTime / 0.25f, 0.f, 1.f) * 0.35f;
        DrawRect(FLinearColor(1.f, 0.f, 0.f, FlashAlpha), 0.f, 0.f, Canvas->SizeX, Canvas->SizeY);
    }
    if (!Canvas) return;

    UFont* Font = GEngine ? GEngine->GetMediumFont() : nullptr;
    if (!Font) return;

    RefreshTargetData();

    DrawTopLeftPanel();
    DrawTopCenterText();
    DrawTopRightPanel();
    DrawCenterReticle();
    DrawBottomLeftPanel();
    DrawBottomRightPanel();

    if (bShowCountdown && CountdownTime >= 0.f)
    {
        FString CountdownStr = FString::Printf(TEXT("TIME: %02d:%02d"),
            (int)CountdownTime / 60, (int)CountdownTime % 60);
        float TextX = (Canvas->SizeX - Font->GetStringSize(*CountdownStr)) * 0.5f;
        DrawShadowedText(CountdownStr, FLinearColor::White, TextX, 50.f, Font);
    }

    if (!CurrentMessage.IsEmpty())
    {
        FString Msg = CurrentMessage.ToUpper();
        float MsgX = (Canvas->SizeX - Font->GetStringSize(*Msg)) * 0.5f;
        float MsgY = Canvas->SizeY * 0.5f + 120.f;
        DrawShadowedText(Msg, FLinearColor(0.f, 1.f, 1.f, 1.f), MsgX, MsgY, Font);
    }
}

void ABulletRushHUD::DrawTopLeftPanel()
{
    if (!Canvas) return;
    UFont* Font = GEngine->GetMediumFont();

    float X = 25.f, Y = 25.f, W = 280.f, H = 90.f;
    DrawPanelBorder(X, Y, W, H);

    DrawShadowedText(TEXT("INTEGRITY"), FLinearColor(0.f, 1.f, 1.f, 1.f), X + 12.f, Y + 8.f, Font);

    float BarX = X + 12.f, BarY = Y + 32.f;
    float BarW = W - 24.f, BarH = 18.f;
    DrawRect(FLinearColor(0.05f, 0.05f, 0.05f, 0.8f), BarX, BarY, BarW, BarH);

    float Percent = CachedMaxHealth > 0 ? (CachedHealth / CachedMaxHealth) : 0.f;

    // Gradient horizontal (segmentos de 1px)
    for (int32 i = 0; i < (int32)(BarW * Percent); i++)
    {
        float Lerp = (float)i / BarW;
        FLinearColor SegColor = (Percent > 0.3f)
            ? FLinearColor::LerpUsingHSV(FLinearColor(0.1f, 0.5f, 1.f, 1.f), FLinearColor(0.f, 1.f, 1.f, 1.f), Lerp)
            : FLinearColor::LerpUsingHSV(FLinearColor(0.5f, 0.0f, 0.0f, 1.f), FLinearColor(1.f, 0.5f, 0.2f, 1.f), Lerp);
        DrawRect(SegColor, BarX + i, BarY, 1.f, BarH);
    }

    // Shimmer animado
    if (Percent > 0.05f)
    {
        float ShimmerX = BarX + FMath::Fmod(TimeAccumulator * 80.f, BarW * Percent);
        if (ShimmerX < BarX + BarW * Percent - 30.f)
        {
            DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.4f), ShimmerX, BarY, 30.f, BarH);
        }
    }

    FString PctText = FString::Printf(TEXT("%d%%"), (int)(Percent * 100.f));
    DrawShadowedText(PctText, FLinearColor::White, BarX + BarW - 50.f, BarY + 1.f, Font);

    FString LivesStr = FString::Printf(TEXT("LIVES: %d"), CachedLives);
    DrawShadowedText(LivesStr, FLinearColor(0.f, 1.f, 1.f, 1.f), X + 12.f, Y + 60.f, Font);
}


void ABulletRushHUD::DrawTopCenterText()
{
    if (!Canvas) return;
    UFont* Font = GEngine->GetMediumFont();

    FString ObjLine = FString::Printf(TEXT("OBJECTIVE: %s"), *CachedObjective);
    float ObjX = (Canvas->SizeX - Font->GetStringSize(*ObjLine)) * 0.5f;
    DrawShadowedText(ObjLine, FLinearColor::White, ObjX, 22.f, Font);
}

void ABulletRushHUD::DrawTopRightPanel()
{
    if (!Canvas) return;
    UFont* Font = GEngine->GetMediumFont();

    float W = 140.f, H = 80.f;
    float X = Canvas->SizeX - W - 25.f;
    float Y = 25.f;
    DrawPanelBorder(X, Y, W, H);

    DrawShadowedText(TEXT("SPEED"), FLinearColor(0.f, 1.f, 1.f, 1.f), X + 12.f, Y + 8.f, Font);

    FString SpeedStr = FString::Printf(TEXT("%.0f u/s"), CachedSpeed);
    DrawShadowedText(SpeedStr, FLinearColor::White, X + 12.f, Y + 32.f, Font);

    FString StatusStr = CachedHealth > 0 ? TEXT("STATUS: ALIVE") : TEXT("STATUS: DEAD");
    FLinearColor StatusColor = CachedHealth > 0
        ? FLinearColor(0.f, 1.f, 0.6f, 1.f)
        : FLinearColor(1.f, 0.3f, 0.3f, 1.f);
    DrawShadowedText(StatusStr, StatusColor, X + 12.f, Y + 56.f, Font);
}

void ABulletRushHUD::DrawCenterReticle()
{
    if (!Canvas) return;
    float CX = Canvas->SizeX * 0.5f;
    float CY = Canvas->SizeY * 0.5f;

    FLinearColor Cyan(0.f, 1.f, 1.f, 0.9f);
    FLinearColor CyanGlow(0.f, 0.7f, 1.f, 0.3f);

    // Círculos concéntricos respirando
    float OuterRadius = 90.f + FMath::Sin(TimeAccumulator * 2.f) * 4.f;
    float MidRadius = 70.f;
    float InnerRadius = 50.f + FMath::Sin(TimeAccumulator * 3.f) * 2.f;

    // Anillo exterior (grueso, pulsante)
    DrawCircle(CX, CY, OuterRadius, 48, 2.f, Cyan);
    // Anillo medio (más fino)
    DrawCircle(CX, CY, MidRadius, 36, 1.5f, FLinearColor(0.f, 1.f, 1.f, 0.5f));
    // Anillo interior (con glow)
    DrawCircle(CX, CY, InnerRadius, 24, 3.f, CyanGlow);

    // Marcas tick (4 puntos cardinales en el anillo medio)
    for (int32 i = 0; i < 4; i++)
    {
        float Angle = i * PI * 0.5f;
        float X1 = CX + FMath::Cos(Angle) * (MidRadius - 6.f);
        float Y1 = CY + FMath::Sin(Angle) * (MidRadius - 6.f);
        float X2 = CX + FMath::Cos(Angle) * (MidRadius + 6.f);
        float Y2 = CY + FMath::Sin(Angle) * (MidRadius + 6.f);
        DrawLine(X1, Y1, X2, Y2, Cyan);
    }

    // Crosshair central
    DrawLine(CX - 15.f, CY, CX - 5.f, CY, FLinearColor::White);
    DrawLine(CX + 5.f, CY, CX + 15.f, CY, FLinearColor::White);
    DrawLine(CX, CY - 15.f, CX, CY - 5.f, FLinearColor::White);
    DrawLine(CX, CY + 5.f, CX, CY + 15.f, FLinearColor::White);
    // Punto central
    DrawRect(FLinearColor::White, CX - 1.5f, CY - 1.5f, 3.f, 3.f);

    if (bHasTarget)
    {
        UFont* Font = GEngine->GetMediumFont();
        FString TargetStr = FString::Printf(TEXT("TARGET: %s [%.0fu]"),
            *CachedTargetName, CachedTargetDist);
        float TgtX = (Canvas->SizeX - Font->GetStringSize(*TargetStr)) * 0.5f;
        DrawShadowedText(TargetStr, FLinearColor(1.f, 0.3f, 0.3f, 1.f), TgtX, CY - 130.f, Font);

        // Barra de vida del target con gradient
        
    }
    else
    {
        UFont* Font = GEngine->GetMediumFont();
        FString ScanStr = TEXT("SCAN: NO TARGET");
        float ScanX = (Canvas->SizeX - Font->GetStringSize(*ScanStr)) * 0.5f;
        DrawShadowedText(ScanStr, FLinearColor(0.5f, 0.5f, 0.5f, 1.f), ScanX, CY - 130.f, Font);
    }
}

void ABulletRushHUD::DrawBottomLeftPanel()
{
    if (!Canvas) return;
    UFont* Font = GEngine->GetMediumFont();

    if (!CachedPlayer.IsValid() || !CachedPlayer->BuffComp) return;

    float W = 260.f;
    float LineH = 22.f;
    int32 BuffCount = CachedPlayer->BuffComp->GetActiveBuffs().Num();
    float H = 30.f + FMath::Min(BuffCount, 5) * LineH + 8.f;
    float X = 25.f;
    float Y = Canvas->SizeY - H - 25.f;

    DrawPanelBorder(X, Y, W, H);
    DrawShadowedText(TEXT("BUFFS"), FLinearColor(0.f, 1.f, 1.f, 1.f), X + 12.f, Y + 6.f, Font);

    int32 Idx = 0;
    for (const auto& Active : CachedPlayer->BuffComp->GetActiveBuffs())
    {
        if (Idx >= 5) break;
        if (!Active.Decorator) continue;

        FString BuffName = Active.Decorator->GetClass()->GetName();
        BuffName.RemoveFromStart(TEXT("U"));
        BuffName.RemoveFromEnd(TEXT("_C"));

        FString Display = FString::Printf(TEXT("[ %s ]"), *BuffName);
        DrawShadowedText(Display, FLinearColor(0.4f, 0.9f, 1.f, 1.f),
            X + 12.f, Y + 28.f + Idx * LineH, Font);
        Idx++;
    }
}

void ABulletRushHUD::DrawBottomRightPanel()
{
    if (!Canvas) return;
    UFont* Font = GEngine->GetMediumFont();

    if (!CachedPlayer.IsValid()) return;

    int32 WeaponCount = CachedPlayer->EquippedWeapons.Num();
    float H = 30.f + WeaponCount * 22.f + 8.f;
    float W = 230.f;
    float X = Canvas->SizeX - W - 25.f;
    float Y = Canvas->SizeY - H - 25.f;

    DrawPanelBorder(X, Y, W, H);
    DrawShadowedText(TEXT("WEAPONS"), FLinearColor(0.f, 1.f, 1.f, 1.f), X + 12.f, Y + 6.f, Font);

    int32 Idx = 0;
    for (UWeaponBaseComponent* Weapon : CachedPlayer->EquippedWeapons)
    {
        if (!Weapon) continue;

        FString WName = GetWeaponDisplayName(Weapon);
        FString Line = FString::Printf(TEXT("[ %s ]"), *WName);

        FLinearColor LineColor = (Weapon == CachedPlayer->CurrentWeapon)
            ? FLinearColor(0.f, 1.f, 0.6f, 1.f)
            : FLinearColor(0.4f, 0.9f, 1.f, 1.f);

        DrawShadowedText(Line, LineColor, X + 12.f, Y + 28.f + Idx * 22.f, Font);
        Idx++;
    }
}

void ABulletRushHUD::RefreshTargetData()
{
    bHasTarget = false;
    CachedTargetName.Empty();
    CachedTargetDist = 0.f;
    CachedSpeed = 0.f;

    if (!CachedPlayer.IsValid() || !GetWorld()) return;

    CachedSpeed = CachedPlayer->GetVelocity().Size();

    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Enemies);

    FVector PlayerLoc = CachedPlayer->GetActorLocation();
    float MinDist = 5000.f;
    AActor* Closest = nullptr;

    for (AActor* E : Enemies)
    {
        if (!E || !IsValid(E)) continue;
        float D = FVector::Dist(PlayerLoc, E->GetActorLocation());
        if (D < MinDist)
        {
            MinDist = D;
            Closest = E;
        }
    }

    if (Closest)
    {
        bHasTarget = true;
        CachedTargetName = Closest->GetName();
        CachedTargetDist = MinDist;
    }
}

FString ABulletRushHUD::GetWeaponDisplayName(UWeaponBaseComponent* Weapon) const
{
    if (!Weapon || !Weapon->FireStrategy) return TEXT("NONE");

    if (Cast<UAutoFireStrategy>(Weapon->FireStrategy.GetObject())) return TEXT("AUTO");
    if (Cast<UVolleyStrategy>(Weapon->FireStrategy.GetObject()))   return TEXT("VOLLEY");
    if (Cast<UPlusFireStrategy>(Weapon->FireStrategy.GetObject())) return TEXT("SPREAD");

    return TEXT("UNKNOWN");
}

void ABulletRushHUD::DrawShadowedText(const FString& Text, FLinearColor Color,
    float X, float Y, UFont* Font, float Scale)
{
    DrawText(Text, FLinearColor(0.f, 0.f, 0.f, 0.8f), X + 2.f, Y + 2.f, Font, Scale);
    DrawText(Text, Color, X, Y, Font, Scale);
}
// ==================== HELPERS DE FORMAS ====================

void ABulletRushHUD::DrawCircle(float CX, float CY, float Radius, int32 Segments, float T, FLinearColor Color)
{
    // Dibuja un círculo hueco de grosor T
    for (int32 i = 0; i < Segments; i++)
    {
        float Angle1 = (float)i / Segments * 2.f * PI;
        float Angle2 = (float)(i + 1) / Segments * 2.f * PI;

        float X1 = CX + FMath::Cos(Angle1) * Radius;
        float Y1 = CY + FMath::Sin(Angle1) * Radius;
        float X2 = CX + FMath::Cos(Angle2) * Radius;
        float Y2 = CY + FMath::Sin(Angle2) * Radius;

        // Dibuja un "grosor" perpendicular a la línea
        float MidX = (X1 + X2) * 0.5f;
        float MidY = (Y1 + Y2) * 0.5f;
        float Dist = FVector2D::Distance(FVector2D(X1, Y1), FVector2D(X2, Y2));
        float Angle = FMath::Atan2(Y2 - Y1, X2 - X1);

        // Cuadrado rotado para simular grosor
        FCanvasTileItem Tile(FVector2D(MidX - T * 0.5f, MidY - Dist * 0.5f),
            FVector2D(T, Dist), Color);
        Tile.Rotation = FRotator(0.f, 0.f, FMath::RadiansToDegrees(Angle));
        Tile.PivotPoint = FVector2D(0.5f, 0.5f);
        Canvas->DrawItem(Tile);
    }
}

void ABulletRushHUD::DrawRoundedRect(float X, float Y, float W, float H, float Radius, FLinearColor Color)
{
    // Relleno del rectángulo redondeado
    // Centro
    DrawRect(Color, X + Radius, Y + Radius, W - 2.f * Radius, H - 2.f * Radius);
    // Bordes verticales (sin esquinas)
    DrawRect(Color, X, Y + Radius, W, H - 2.f * Radius);
    // Esquinas como círculos
    DrawCircleFilled(X + Radius, Y + Radius, Radius, 16, Color);
    DrawCircleFilled(X + W - Radius, Y + Radius, Radius, 16, Color);
    DrawCircleFilled(X + Radius, Y + H - Radius, Radius, 16, Color);
    DrawCircleFilled(X + W - Radius, Y + H - Radius, Radius, 16, Color);
}

void ABulletRushHUD::DrawCircleFilled(float CX, float CY, float Radius, int32 Segments, FLinearColor Color)
{
    if (!GWhiteTexture) return;

    FCanvasNGonItem NGon(
        FVector2D(CX, CY),
        FVector2D(Radius, Radius),
        Segments,
        GWhiteTexture,
        Color
    );
    NGon.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(NGon);
}

void ABulletRushHUD::DrawRoundedRectBorder(float X, float Y, float W, float H, float Radius, float T, FLinearColor Color)
{
    // 4 lados rectos
    DrawRect(Color, X + Radius, Y, W - 2.f * Radius, T);                          // top
    DrawRect(Color, X + Radius, Y + H - T, W - 2.f * Radius, T);                  // bottom
    DrawRect(Color, X, Y + Radius, T, H - 2.f * Radius);                          // left
    DrawRect(Color, X + W - T, Y + Radius, T, H - 2.f * Radius);                  // right

    // 4 esquinas como arcos
    DrawCircleArc(X + Radius, Y + Radius, Radius, PI, 1.5f * PI, T, Color);        // sup-izq
    DrawCircleArc(X + W - Radius, Y + Radius, Radius, 1.5f * PI, 2.f * PI, T, Color);  // sup-der
    DrawCircleArc(X + Radius, Y + H - Radius, Radius, 0.5f * PI, PI, T, Color);   // inf-izq
    DrawCircleArc(X + W - Radius, Y + H - Radius, Radius, 0, 0.5f * PI, T, Color); // inf-der
}

void ABulletRushHUD::DrawCircleArc(float CX, float CY, float Radius,
    float StartAngle, float EndAngle,
    float T, FLinearColor Color)
{
    int32 Segments = 12;
    float Range = EndAngle - StartAngle;
    for (int32 i = 0; i < Segments; i++)
    {
        float A1 = StartAngle + (float)i / Segments * Range;
        float A2 = StartAngle + (float)(i + 1) / Segments * Range;

        float X1 = CX + FMath::Cos(A1) * Radius;
        float Y1 = CY + FMath::Sin(A1) * Radius;
        float X2 = CX + FMath::Cos(A2) * Radius;
        float Y2 = CY + FMath::Sin(A2) * Radius;

        float MidX = (X1 + X2) * 0.5f;
        float MidY = (Y1 + Y2) * 0.5f;
        float Dist = FVector2D::Distance(FVector2D(X1, Y1), FVector2D(X2, Y2));
        float Angle = FMath::Atan2(Y2 - Y1, X2 - X1);

        FCanvasTileItem Tile(FVector2D(MidX - T * 0.5f, MidY - Dist * 0.5f),
            FVector2D(T, Dist), Color);
        Tile.Rotation = FRotator(0.f, 0.f, FMath::RadiansToDegrees(Angle));
        Tile.PivotPoint = FVector2D(0.5f, 0.5f);
        Canvas->DrawItem(Tile);
    }
}