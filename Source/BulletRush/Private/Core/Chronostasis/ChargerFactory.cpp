#include "Core/Chronostasis/ChargerFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisCharger.h"

AEnemyBase* UChargerFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = ChargerClass ? ChargerClass.Get() : AChronostasisCharger::StaticClass();
	AEnemyBase* C = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

	UE_LOG(LogTemp, Warning, TEXT("UChargerFactory::CreateEnemy: Spawning Charger at %s. Success: %s"), *Location.ToString(), C ? TEXT("True") : TEXT("False"));
	return C;
}
