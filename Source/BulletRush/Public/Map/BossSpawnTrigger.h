#pragma once

#include "CoreMinimal.h"
#include "Map/LevelPortal.h"
#include "BossSpawnTrigger.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class ABloodseekerBoss;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossSpawned, ABloodseekerBoss*, Boss);

UCLASS()
class BULLETRUSH_API ABossSpawnTrigger : public AActor
{
    GENERATED_BODY()

public:
    ABossSpawnTrigger();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    void EnableTrigger();

public:
    FOnBossSpawned OnBossSpawned;

private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* TriggerMesh;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* TriggerVolume;

    UPROPERTY(EditAnywhere, Category = "Boss Trigger")
    TSubclassOf<ABloodseekerBoss> BossClass;

    UPROPERTY(EditAnywhere, Category = "Boss Trigger")
    float ActivationDelay;

    UPROPERTY(EditAnywhere, Category = "Boss Trigger")
    UStaticMesh* TriggerVisualMesh;

    float PulseTime;
    bool bIsReady;
};
