#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttackStrategy.generated.h"

/**
* Estructura de parámetros para ataques, puede ser extendida según necesidades
* Count: Cantidad de balas a disparar
* Speed: Velocidad de las balas
* DelayAfter: Tiempo a esperar después de ejecutar este ataque antes de pasar al siguiente
* SpecialParam: Parámetro especial para cada patrón (ej. rotación inicial para círculo, intervalo entre balas para ráfaga, etc.)
* Origin: Punto de origen para el ataque, puede ser el jefe o una ubicación específica
* Damage: Daño que infligirán las balas generadas por este ataque
*/
USTRUCT(BlueprintType)
struct FAttackParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Speed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DelayAfter = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SpecialParam = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector Origin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Damage = 10.f;
};

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class BULLETRUSH_API UAttackStrategy : public UObject
{
	GENERATED_BODY()

public:
	// Ejecuta el ataque utilizando el Spawner para generar las balas según los parámetros definidos en Params. 
	// Cada estrategia implementará su propia lógica para distribuir las balas en el espacio, calcular direcciones, etc.
	virtual void Execute(class UBulletSpawnerComponent* Spawner, const FAttackParams& Params) PURE_VIRTUAL(UAttackStrategy::Execute, );
};