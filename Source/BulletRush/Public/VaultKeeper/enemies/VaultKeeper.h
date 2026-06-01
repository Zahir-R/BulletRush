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

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Ciclo ---
    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Cycle")
    float OpenDuration = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Cycle")
    float ClosedDuration = 4.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Heal")
    float HealRate = 15.0f;

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
};