#include "Core/Chronostasis/MassFactory.h"
#include "Engine/World.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"

AEnemyBase* UMassFactory::CreateEnemy(UWorld* World, const FVector& Location)
{
	if (!World) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UClass* ClassToSpawn = MassClass ? MassClass.Get() : AChronostasisMass::StaticClass();
	AEnemyBase* M = World->SpawnActor<AEnemyBase>(ClassToSpawn, Location, FRotator::ZeroRotator, Params);

	UE_LOG(LogTemp, Warning, TEXT("UMassFactory::CreateEnemy: Spawning Mass at %s. Success: %s"), *Location.ToString(), M ? TEXT("True") : TEXT("False"));
	return M;
}
