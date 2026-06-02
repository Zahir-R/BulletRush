#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "EnemyWaveSpawn.generated.h"

class AEnemyBase;

UCLASS()
class BULLETRUSH_API AEnemyWaveSpawn : public APawn
{
	GENERATED_BODY()

public:
	AEnemyWaveSpawn	();

protected:
	virtual void BeginPlay() override;


	UPROPERTY()
	TArray<AEnemyBase*> ActiveEnemies;

	UFUNCTION()
	void OnEnemyKilled(AEnemyBase* DeadEnemy);


	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> EnemyClass;

	void SpawnWave();
};