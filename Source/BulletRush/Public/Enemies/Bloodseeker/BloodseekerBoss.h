// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "BloodseekerBoss.generated.h"

class APlayingPlayer;
/**
 * 
 */
UCLASS()
class BULLETRUSH_API ABloodseekerBoss : public ABossBase
{
	GENERATED_BODY()
public:
	ABloodseekerBoss();
protected:

    virtual void BeginPlay() override;

public:
    
    virtual void Tick(float DeltaTime) override;

    virtual void Attack() override;
private:
    
    UPROPERTY()
    APlayingPlayer* TargetPlayer;

    
    FVector InitialLocation;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Movement", meta = (AllowPrivateAccess = "true"))
    float MovementAmplitude;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Movement", meta = (AllowPrivateAccess = "true"))
    float MovementFrequency;

    //Mecanica Rupture
    bool bIsRuptureActive;
    FVector LastPlayerLocation;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Rupture", meta = (AllowPrivateAccess = "true"))
    float RuptureDamageMultiplier;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Rupture", meta = (AllowPrivateAccess = "true"))
    float RuptureDuration;

    FTimerHandle RuptureTimerHandle;

    //Funciones internas
    void ExecuteRuptureAttack();
    void DeactivateRupture();

    
    TArray<FAttackStep> CustomBurstCombo;
};
