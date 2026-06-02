#include "Core/Chronostasis/DroneFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"

AEnemyBase* UDroneFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = DroneClass ? DroneClass.Get() : AChronostasisDrone::StaticClass();
	AEnemyBase* D = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

	UE_LOG(LogTemp, Warning, TEXT("UDroneFactory::CreateEnemy: Spawning Drone at %s. Success: %s"), *Location.ToString(), D ? TEXT("True") : TEXT("False"));
	return D;
}
