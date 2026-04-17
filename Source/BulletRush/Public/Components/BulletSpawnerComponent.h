#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Subsystems/ProjectilesSubsystem.h"
#include "BulletSpawnerComponent.generated.h"

enum class EAttackType : uint8
{
	Circle,
	Spiral,
	Burst
	// otros tipos de ataque
};

struct FAttackStep
{	
	EAttackType Type;
	int32 BulletCount;
	float Speed;
	float DelayAfter;
	float SpecialParam;
	bool bUseBossLocation; // Si genera en el boss o en otro lugar
	FVector CustomOrigin; // Si bUseBossLocation es false
	float Damage;

	/**
	* Constructor por defecto, genera un ataque circular con 10 balas, velocidad 500, delay de 1 segundo, sin parámetros especiales y origen en el boss. Esto es solo para facilitar la creación de ataques simples, se pueden usar los otros constructores para más control.
	* Los parámetros, en orden, son Tipo de ataque, Cantidad de balas, Velocidad, Delay después del ataque, Parámetro especial (dependiendo del tipo de ataque), Si usa la ubicación del boss o no, y la ubicación personalizada si no se usa la del boss.
	*/
	FAttackStep() : Type(EAttackType::Circle), BulletCount(10), Speed(500.0f), DelayAfter(1.0f), SpecialParam(0.0f), bUseBossLocation(true), CustomOrigin(FVector::ZeroVector), Damage(10.0f) {}

	// * Constructor para ataques desde boss. En cuyo caso el Origen es ZeroVector
	FAttackStep(EAttackType InType, int32 InCount, float InSpeed, float InDelay, float InSpecial = 0.0f, float InDamage = 10.0f) 
		: Type(InType), BulletCount(InCount), Speed(InSpeed), DelayAfter(InDelay), SpecialParam(InSpecial), bUseBossLocation(true), CustomOrigin(FVector::ZeroVector), Damage(InDamage) {}

	// * Constructor para ataques en ubicación específica, donde el origen es diferente del boss
	FAttackStep(EAttackType InType, int32 InCount, float InSpeed, float InDelay, FVector InOrigin, float InSpecial = 0.0f, float InDamage = 10.0f)
		: Type(InType), BulletCount(InCount), Speed(InSpeed), DelayAfter(InDelay), SpecialParam(InSpecial), bUseBossLocation(false), CustomOrigin(InOrigin), Damage(InDamage) {}
};

class IAttackStrategy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API UBulletSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBulletSpawnerComponent();
	/**
	* Empieza una secuencia de ataques, cada uno definido por un FAttackStep. El componente se encargará de ejecutar cada paso en orden, respetando los tiempos de delay y utilizando las estrategias de ataque correspondientes al tipo de ataque definido en cada paso.
	*/
	void StartSequence(const TArray<FAttackStep>& NewSequence);
	/**
	* Genera las balas en el mundo. Esta función es llamada por las estrategias de ataque para crear las balas con la dirección y velocidad adecuadas. El origen puede ser el jefe o una ubicación específica, dependiendo de los parámetros del ataque.
	*/
	void InternalSpawn(FVector Origin, FVector Direction, float Speed, float Damage);

	void StopCurrentSequence();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // Ni idea si usemos esto después, lo dejo por si acaso
	

private:
	//float CurrentSpiralAngle = 0.0f;
	//FTimerHandle BurstTimerHandle;
	//int32 BulletsLeftToBurst;
	//float CachedBurstSpeed;

	//void ExecuteBurstStep();

	TArray<FAttackStep> CurrentSequence;
	int32 CurrentStepIndex;
	FTimerHandle SequenceTimerHandle;
	/**
	* Ejecuta el siguiente paso en la secuencia de ataques. Esta función es llamada automáticamente después de cada ataque, utilizando el tiempo de delay definido en el paso actual para programar la ejecución del siguiente paso.
	*/
	void ExecuteNextStep();
	TMap<EAttackType, TSharedPtr<IAttackStrategy>> AttackRegist;

	UProjectilesSubsystem* ProjectilesSubsystem;

	bool bIsPlayerSource = false;
};
