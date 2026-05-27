// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Chronostasis/DefChronostasisEnemyFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"

AEnemyBase* UDefChronostasisEnemyFactory::CreateDrone(UWorld* World, const FVector& Location)
{
    if (!World) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UClass* ClassToSpawn = DroneClass ? DroneClass.Get() : AChronostasisDrone::StaticClass();
    AEnemyBase* D = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

    UE_LOG(LogTemp, Warning, TEXT("UDefChronostasisEnemyFactory::CreateDrone: Spawning Drone at %s. Success: %s"), *Location.ToString(), D ? TEXT("True") : TEXT("False"));
    return D;
}

AEnemyBase* UDefChronostasisEnemyFactory::CreateMass(UWorld* World, const FVector& Location)
{
    if (!World) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UClass* ClassToSpawn = MassClass ? MassClass.Get() : AChronostasisMass::StaticClass();
    AEnemyBase* M = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

    UE_LOG(LogTemp, Warning, TEXT("UDefChronostasisEnemyFactory::CreateMass: Spawning Mass at %s. Success: %s"), *Location.ToString(), M ? TEXT("True") : TEXT("False"));
    return M;
}

AEnemyBase* UDefChronostasisEnemyFactory::CreateExpansive(UWorld* World, const FVector& Location)
{
    if (!World) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UClass* ClassToSpawn = ExpansiveClass ? ExpansiveClass.Get() : AChronostasisExpansive::StaticClass();
    AEnemyBase* E = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

    UE_LOG(LogTemp, Warning, TEXT("UDefChronostasisEnemyFactory::CreateExpansive: Spawning Expansive at %s. Success: %s"), *Location.ToString(), E ? TEXT("True") : TEXT("False"));
    return E;
}