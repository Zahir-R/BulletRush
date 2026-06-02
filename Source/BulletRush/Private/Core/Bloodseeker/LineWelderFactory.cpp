#include "Core/Bloodseeker/LineWelderFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Bloodseeker/LineWelderEnemy.h"

AEnemyBase* ULineWelderFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = LineWelderClass ? LineWelderClass.Get() : ALineWelderEnemy::StaticClass();
	AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(ClassToSpawn, FTransform(Location), SpawnParams);

	UE_LOG(LogTemp, Warning, TEXT("[LineWelderFactory] Spawned LineWelder at %s. Success: %s"), *Location.ToString(), Enemy ? TEXT("True") : TEXT("False"));
	return Enemy;
}
