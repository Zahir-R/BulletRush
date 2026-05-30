#include "Core/Chronostasis/LinkerFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisLinker.h"

AEnemyBase* ULinkerFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = LinkerClass ? LinkerClass.Get() : AChronostasisLinker::StaticClass();
	AEnemyBase* L = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

	UE_LOG(LogTemp, Warning, TEXT("ULinkerFactory::CreateEnemy: Spawning Linker at %s. Success: %s"), *Location.ToString(), L ? TEXT("True") : TEXT("False"));
	return L;
}
