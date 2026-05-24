#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

class AEnemyBase;

class IChronostasisEnemyFactory
{
public:
    virtual ~IChronostasisEnemyFactory() {}
    virtual AEnemyBase* CreateDrone(UWorld* World, const FVector& Location) = 0;
    virtual AEnemyBase* CreateMass(UWorld* World, const FVector& Location) = 0;
    virtual AEnemyBase* CreateExpansive(UWorld* World, const FVector& Location) = 0;
};

// Concrete factory
class FChronostasisEnemyFactory : public IChronostasisEnemyFactory
{
public:
    TSubclassOf<AActor> DroneClass;
    TSubclassOf<AActor> MassClass;
    TSubclassOf<AActor> ExpansiveClass;

    FChronostasisEnemyFactory() {}
    virtual AEnemyBase* CreateDrone(UWorld* World, const FVector& Location) override;
    virtual AEnemyBase* CreateMass(UWorld* World, const FVector& Location) override;
    virtual AEnemyBase* CreateExpansive(UWorld* World, const FVector& Location) override;
};
