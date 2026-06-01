#include "Core/Bloodseeker/BloodseekerEnemyFactory.h"
#include "Enemies/Bloodseeker/KamikazeEnemy.h"
#include "Enemies/Bloodseeker/LineWelderEnemy.h"
#include "Enemies/Bloodseeker/GravitySiphonEnemy.h"
#include "Engine/World.h"

UBloodseekerEnemyFactory::UBloodseekerEnemyFactory()
{
}

AKamikazeEnemy* UBloodseekerEnemyFactory::CreateKamikaze(UWorld* World, const FVector& SpawnLocation)
{
    if (!World) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AKamikazeEnemy* NewEnemy = World->SpawnActor<AKamikazeEnemy>(
        KamikazeClass ? KamikazeClass : AKamikazeEnemy::StaticClass(),
        FTransform(SpawnLocation),
        SpawnParams
    );

    return NewEnemy;
}

ALineWelderEnemy* UBloodseekerEnemyFactory::CreateLineWelder(UWorld* World, const FVector& SpawnLocation)
{
    if (!World) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ALineWelderEnemy* NewEnemy = World->SpawnActor<ALineWelderEnemy>(
        LineWelderClass ? LineWelderClass : ALineWelderEnemy::StaticClass(),
        FTransform(SpawnLocation),
        SpawnParams
    );

    return NewEnemy;
}

AGravitySiphonEnemy* UBloodseekerEnemyFactory::CreateGravitySiphon(UWorld* World, const FVector& SpawnLocation)
{
    if (!World) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AGravitySiphonEnemy* NewEnemy = World->SpawnActor<AGravitySiphonEnemy>(
        GravitySiphonClass ? GravitySiphonClass : AGravitySiphonEnemy::StaticClass(),
        FTransform(SpawnLocation),
        SpawnParams
    );

    return NewEnemy;
}
