#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "ChronostasisFactoryEnemy.generated.h"

class AEnemyBase;

UCLASS(Abstract, Blueprintable, BlueprintType)
class BULLETRUSH_API UChronostasisFactoryEnemy : public UObject
{
	GENERATED_BODY()

public:
	virtual AEnemyBase* CreateDrone(UWorld* World, const FVector& Location) PURE_VIRTUAL(UChronostasisFactoryEnemy::CreateDrone, return nullptr;);
	virtual AEnemyBase* CreateMass(UWorld* World, const FVector& Location) PURE_VIRTUAL(UChronostasisFactoryEnemy::CreateMass, return nullptr;);
	virtual AEnemyBase* CreateExpansive(UWorld* World, const FVector& Location) PURE_VIRTUAL(UChronostasisFactoryEnemy::CreateExpansive, return nullptr;);
};