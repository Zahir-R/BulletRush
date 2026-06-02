#include "Core/Chronostasis/GenericEnemyFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"

AEnemyBase* UGenericEnemyFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = EnemyClass ? EnemyClass.Get() : AEnemyBase::StaticClass();
	AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

	return Enemy;
}
