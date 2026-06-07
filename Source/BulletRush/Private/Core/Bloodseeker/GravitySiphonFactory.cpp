#include "Core/Bloodseeker/GravitySiphonFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Bloodseeker/GravitySiphonEnemy.h"

AEnemyBase* UGravitySiphonFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = GravitySiphonClass ? GravitySiphonClass : AGravitySiphonEnemy::StaticClass();
	AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(ClassToSpawn, FTransform(Location), SpawnParams);

	UE_LOG(LogTemp, Warning, TEXT("[GravitySiphonFactory] Spawned GravitySiphon at %s. Success: %s"), *Location.ToString(), Enemy ? TEXT("True") : TEXT("False"));
	return Enemy;
}
