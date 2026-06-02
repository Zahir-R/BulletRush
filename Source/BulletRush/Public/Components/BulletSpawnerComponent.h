#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "UObject/StrongObjectPtr.h"
#include "BulletSpawnerComponent.generated.h"

class IAttackStrategy;

enum class EAttackType : uint8
{
	Circle,
	Sphere,
	Spiral,
	Burst,
	SurroundingBullets,
};

struct FAttackStep
{	
	EAttackType Type = EAttackType::Circle;
	int32 BulletCount = 10;
	float Speed = 500.0f;
	float DelayAfter = 1.0f;
	float SpecialParam = 0.0f;
	bool bUseBossLocation = true;
	FVector CustomOrigin = FVector::ZeroVector;
	float TimeBetweenShots = 0.1f;
	FVector BulletScale = FVector(0.4f, 0.4f, 0.4f);
	float Damage = 10.0f;

	/**
	* Constructor por defecto, genera un ataque circular con 10 balas, velocidad 500, delay de 1 segundo, sin par�metros especiales y origen en el boss. Esto es solo para facilitar la creaci�n de ataques simples, se pueden usar los otros constructores para m�s control.
	* Los par�metros, en orden, son Tipo de ataque, Cantidad de balas, Velocidad, Delay despu�s del ataque, Par�metro especial (dependiendo del tipo de ataque), Si usa la ubicaci�n del boss o no, y la ubicaci�n personalizada si no se usa la del boss.
	*/
	FAttackStep()	{}

	/* Constructor para ataques desde boss. En cuyo caso el Origen es ZeroVector
	FAttackStep(EAttackType InType, int32 InCount, float InSpeed, float InDelay, float InSpecial = 0.0f, float InDamage = 10.0f) 
		: Type(InType), BulletCount(InCount), Speed(InSpeed), DelayAfter(InDelay), SpecialParam(InSpecial), bUseBossLocation(true), CustomOrigin(FVector::ZeroVector), Damage(InDamage) {}

*/	// * Constructor para ataques en ubicaci�n espec�fica, donde el origen es diferente del boss
	FAttackStep(EAttackType InType, int32 InCount, float InSpeed, float InDelay, float InTime, float InSpecial = 0.0f, FVector InScale = FVector(0.4f), float InDamage = 10.0f)
		: Type(InType), BulletCount(InCount), Speed(InSpeed), DelayAfter(InDelay), TimeBetweenShots(InTime), SpecialParam(InSpecial), BulletScale(InScale), Damage(InDamage) {
	}

	// Constructor para ataques en ubicación específica
	FAttackStep(EAttackType InType, int32 InCount, float InSpeed, float InDelay, FVector InOrigin, float InTime = 0.1f, float InSpecial = 0.0f, float InDamage = 10.0f)
		: Type(InType), BulletCount(InCount), Speed(InSpeed), DelayAfter(InDelay), bUseBossLocation(false), CustomOrigin(InOrigin), TimeBetweenShots(InTime), SpecialParam(InSpecial), Damage(InDamage) {
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API UBulletSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBulletSpawnerComponent();
	/**
	* Empieza una secuencia de ataques, cada uno definido por un FAttackStep. El componente se encargar� de ejecutar cada paso en orden, respetando los tiempos de delay y utilizando las estrategias de ataque correspondientes al tipo de ataque definido en cada paso.
	*/
	void StartSequence(const TArray<FAttackStep>& NewSequence);
	/**
	* Genera las balas en el mundo. Esta funci�n es llamada por las estrategias de ataque para crear las balas con la direcci�n y velocidad adecuadas. El origen puede ser el jefe o una ubicaci�n espec�fica, dependiendo de los par�metros del ataque.
	*/
	void InternalSpawn(FVector Origin, FVector Direction, float Speed, float Damage, FVector Scale = FVector(0.4f));

	void StopCurrentSequence();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // Ni idea si usemos esto despu�s, lo dejo por si acaso
	
	class UProjectilesSubsystem* GetProjectilesSubsystem() const { return ProjectilesSubsystem; }

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
	* Ejecuta el siguiente paso en la secuencia de ataques. Esta funci�n es llamada autom�ticamente despu�s de cada ataque, utilizando el tiempo de delay definido en el paso actual para programar la ejecuci�n del siguiente paso.
	*/
	void ExecuteNextStep();
	TMap<EAttackType, TStrongObjectPtr<UAttackStrategy>> AttackRegist;

	UProjectilesSubsystem* ProjectilesSubsystem;

	bool bIsPlayerSource = false;
};
