#pragma once
#include "CoreMinimal.h"
#include "TimerManager.h"

class UBulletSpawnerComponent;
/**
* Estructura de parámetros para ataques, puede ser extendida según necesidades
* Count: Cantidad de balas a disparar
* Speed: Velocidad de las balas
* DelayAfter: Tiempo a esperar después de ejecutar este ataque antes de pasar al siguiente
* SpecialParam: Parámetro especial para cada patrón (ej. rotación inicial para círculo, intervalo entre balas para ráfaga, etc.)
* Origin: Punto de origen para el ataque, puede ser el jefe o una ubicación específica
* Damage: Daño que infligirán las balas generadas por este ataque
*/
struct FAttackParams
{
	int32 Count;
	float Speed;
	float DelayAfter;
	float SpecialParam; // Rotacion, intervalo, o algún otro para diferente patrón
	FVector Origin;
	float Damage;
};

class IAttackStrategy
{
public:
	virtual ~IAttackStrategy() {}
	// Ejecuta el ataque utilizando el Spawner para generar las balas según los parámetros definidos en Params. Cada estrategia implementará su propia lógica para distribuir las balas en el espacio, calcular direcciones, etc.
	virtual void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) = 0;
};

class FCircleAttack : public IAttackStrategy
{
public:
	// Ejecuta un ataque circular, generando balas distribuidas en un círculo alrededor del origen definido en Params. La cantidad de balas, velocidad y cualquier otro parámetro relevante se toman de Params. Ver UBulletSpawnerComponent para detalles.
	void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};

class FSpiralAttack : public IAttackStrategy
{
public:
	// Ejecuta un ataque en espiral, generando balas que se distribuyen en una espiral alrededor del origen definido en Params. La cantidad de balas, velocidad, rotación inicial y cualquier otro parámetro relevante se toman de Params. Ver UBulletSpawnerComponent para detalles.
	void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};


class FBurstAttack : public IAttackStrategy
{
public:
	// Ejecuta un ataque de ráfaga, generando un número definido de balas en rápida sucesión desde el origen definido en Params. La cantidad de balas, velocidad, intervalo entre balas y cualquier otro parámetro relevante se toman de Params. Ver UBulletSpawnerComponent para detalles.
	void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};