#include "Core/Chronostasis/ChronostasisEnemyFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"

AEnemyBase* FChronostasisEnemyFactory::CreateDrone(UWorld* World, const FVector& Location)
{
    if (!World) return nullptr;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AChronostasisDrone* D = World->SpawnActor<AChronostasisDrone>(AChronostasisDrone::StaticClass(), Location, FRotator::ZeroRotator, Params);
    UE_LOG(LogTemp, Warning, TEXT("FChronostasisEnemyFactory::CreateDrone: Spawning Drone at %s. Success: %s"), *Location.ToString(), D ? TEXT("True") : TEXT("False"));
    return D;
}

AEnemyBase* FChronostasisEnemyFactory::CreateMass(UWorld* World, const FVector& Location)
{
    if (!World) return nullptr;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AChronostasisMass* M = World->SpawnActor<AChronostasisMass>(AChronostasisMass::StaticClass(), Location, FRotator::ZeroRotator, Params);
    UE_LOG(LogTemp, Warning, TEXT("FChronostasisEnemyFactory::CreateMass: Spawning Mass at %s. Success: %s"), *Location.ToString(), M ? TEXT("True") : TEXT("False"));
    return M;
}

AEnemyBase* FChronostasisEnemyFactory::CreateExpansive(UWorld* World, const FVector& Location)
{
    if (!World) return nullptr;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AChronostasisExpansive* E = World->SpawnActor<AChronostasisExpansive>(AChronostasisExpansive::StaticClass(), Location, FRotator::ZeroRotator, Params);
    UE_LOG(LogTemp, Warning, TEXT("FChronostasisEnemyFactory::CreateExpansive: Spawning Expansive at %s. Success: %s"), *Location.ToString(), E ? TEXT("True") : TEXT("False"));
    return E;
}
