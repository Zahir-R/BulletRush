#include "Core/Bloodseeker/KamikazeFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Bloodseeker/KamikazeEnemy.h"

AEnemyBase* UKamikazeFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = KamikazeClass ? KamikazeClass : AKamikazeEnemy::StaticClass();
	AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(ClassToSpawn, FTransform(Location), SpawnParams);

	UE_LOG(LogTemp, Warning, TEXT("[KamikazeFactory] Spawned Kamikaze at %s. Success: %s"), *Location.ToString(), Enemy ? TEXT("True") : TEXT("False"));
	return Enemy;
}
