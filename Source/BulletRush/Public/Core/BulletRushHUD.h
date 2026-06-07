#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/HUD.h"
#include "BulletRushHUD.generated.h"

class APlayingPlayer;
class UHealthComponent;
class UBuffComponent;
class UWeaponBaseComponent;
class AEnemyBase;
class FCanvasTriangleItem;
UCLASS()
class BULLETRUSH_API ABulletRushHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    void SetCountdown(float TimeLeft);
    void ShowMessage(const FString& Msg, float Duration);

protected:
    void BindToPlayer();
    UFUNCTION() void OnPlayerHealthChanged(float NewHealth);
    UFUNCTION() void OnPlayerDeath();

    // Paneles del HUD
    void DrawTopLeftPanel();
    void DrawTopCenterText();
    void DrawTopRightPanel();
    void DrawCenterReticle();
    void DrawBottomLeftPanel();
    void DrawBottomRightPanel();
    void DrawPanelBorder(float X, float Y, float W, float H, float GlowAlpha = 0.3f, float CornerRadius = 8.f);

    void DrawRoundedRect(float X, float Y, float W, float H, float Radius, FLinearColor Color);
    void DrawCircleFilled(float CX, float CY, float Radius, int32 Segments, FLinearColor Color);
    void DrawRoundedRectBorder(float X, float Y, float W, float H, float Radius, float T, FLinearColor Color);
    void DrawCircleArc(float CX, float CY, float Radius, float StartAngle, float EndAngle, float T, FLinearColor Color);
    void DrawCircle(float CX, float CY, float Radius, int32 Segments, float T, FLinearColor Color);

    // Helpers de estado
    void RefreshTargetData();
    FString GetWeaponDisplayName(UWeaponBaseComponent* Weapon) const;

    // Cache existente
    float CachedHealth = 0.f;
    float CachedMaxHealth = 100.f;
    int32 CachedLives = 0;
    int32 CachedDeaths = 0;
    float CountdownTime = -1.f;
    bool bShowCountdown = false;
    FString CurrentMessage;
    float MessageTimeRemaining = 0.f;
    FTimerHandle MessageTimerHandle;
    void ClearMessage();
    TWeakObjectPtr<APlayingPlayer> CachedPlayer;

    // Animación
    float TimeAccumulator = 0.f;
    float DamageFlashTime = 0.f;

    // Sombra de texto (helper)
    void DrawShadowedText(const FString& Text, FLinearColor Color, float X, float Y, UFont* Font, float Scale = 1.f);

    // Cache nuevo
    float CachedSpeed = 0.f;
    FString CachedObjective;
    FString CachedTargetName;
    float CachedTargetDist = 0.f;
    bool bHasTarget = false;
};