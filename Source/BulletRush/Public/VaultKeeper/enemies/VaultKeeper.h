#pragma once
#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "VaultKeeper.generated.h"

UCLASS()
class BULLETRUSH_API AVaultKeeper : public ABossBase
{
    GENERATED_BODY()

public:
    AVaultKeeper();
    void DestroyOneWeakPoint();

    virtual FLinearColor GetHealthBarColor() const override;
    virtual FLinearColor GetHealthBarColorLow() const override;
    virtual FVector2D GetHealthBarSize() const override;
    virtual bool IsBoss() const override { return true; }
    virtual FString GetBossDisplayName() const override;
    virtual bool ShouldShowBossName() const override { return true; }
    virtual float GetHealthBarVerticalOffset() const override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, Category = "VaultKeeper|Visuals")
    UStaticMeshComponent* VaultMesh;

    // --- Ciclo ---
    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Cycle")
    float OpenDuration = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Cycle")
    float ClosedDuration = 4.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Heal")
    float HealRate = 35.0f;

    UPROPERTY(VisibleAnywhere, Category = "VaultKeeper|State")
    bool bIsOpen = false;

    UPROPERTY(VisibleAnywhere, Category = "VaultKeeper|State")
    bool bIsStunned = false;

    // --- Materiales cuerpo ---
    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Visuals")
    UMaterialInterface* OpenMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Visuals")
    UMaterialInterface* ClosedMaterial;

    // --- Materiales WeakPoints ---
    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Visuals")
    UMaterialInterface* WPOpenMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Visuals")
    UMaterialInterface* WPClosedMaterial;

    // --- Movimiento ---
    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Movement")
    float HoverAmplitude = 20.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Movement")
    float HoverFrequency = 1.5f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Movement")
    float RotationRate = 30.0f;

    FTimerHandle CycleTimer;
    FTimerHandle HealTimer;

    float HomeZ = 0.0f;
    float CurrentZ = 0.0f;
    int32 OpenAttackIndex = 0;

    void Open();
    void Close();
    void ClearAllTimers();
    void RegenerateWeakPoints();
    void ApplyPassiveHeal();
    void RageAttack();
    void UpdateWeakPointMaterials(bool bOpen);

    // Overrides
    virtual void ChangeState(UBossState* NewState) override;
    virtual void HandleWeakPointDestroyed() override;
    virtual void Attack() override;
    virtual void Die() override;



private:
    TArray<UWeakPointComponent*> CachedWeakPoints;
    APawn* CachedPlayer = nullptr;
    float WPOrbitAngle = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Movement")
    float WPOrbitSpeed = 90.0f; // grados por segundo

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Movement")
    float WPOrbitRadius = 200.0f;
};
