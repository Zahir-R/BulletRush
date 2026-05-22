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

protected:
    virtual void BeginPlay() override;

    // --- Ciclo Abierto/Cerrado ---
    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Cycle")
    float OpenDuration = 6.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Cycle")
    float ClosedDuration = 4.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeper|Heal")
    float HealRate = 75.0f;

    UPROPERTY(VisibleAnywhere, Category = "VaultKeeper|State")
    bool bIsOpen = false;

    FTimerHandle CycleTimer;
    FTimerHandle HealTimer;

    void Open();
    void Close();
    void RegenerateWeakPoints();
    void ApplyPassiveHeal();

    // --- Overrides ---
    virtual void SetBossState(EBossState NewState) override;
    virtual void HandleWeakPointDestroyed() override;
    virtual void Attack() override;
    virtual void Die() override;

private:
    TArray<UWeakPointComponent*> CachedWeakPoints;
    APawn* CachedPlayer = nullptr;
};