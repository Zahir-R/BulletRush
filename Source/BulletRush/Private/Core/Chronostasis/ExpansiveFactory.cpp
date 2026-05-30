#include "Core/Chronostasis/ExpansiveFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"

AEnemyBase* UExpansiveFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = ExpansiveClass ? ExpansiveClass.Get() : AChronostasisExpansive::StaticClass();
	AEnemyBase* E = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

	UE_LOG(LogTemp, Warning, TEXT("UExpansiveFactory::CreateEnemy: Spawning Expansive at %s. Success: %s"), *Location.ToString(), E ? TEXT("True") : TEXT("False"));
	return E;
}
