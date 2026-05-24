#pragma once
#include "CoreMinimal.h"
#include "TimerManager.h"

class UBulletSpawnerComponent;
/**
* Estructura de par�metros para ataques, puede ser extendida seg�n necesidades
* Count: Cantidad de balas a disparar
* Speed: Velocidad de las balas
* DelayAfter: Tiempo a esperar despu�s de ejecutar este ataque antes de pasar al siguiente
* SpecialParam: Par�metro especial para cada patr�n (ej. rotaci�n inicial para c�rculo, intervalo entre balas para r�faga, etc.)
* Origin: Punto de origen para el ataque, puede ser el jefe o una ubicaci�n espec�fica
* Damage: Da�o que infligir�n las balas generadas por este ataque
*/
struct FAttackParams
{
	int32 Count;
	float Speed;
	float DelayAfter;
	float SpecialParam; // Rotacion para c�rculo, intervalo para r�faga, o alg�n otro para diferente patr�n
	FVector Origin;
	float Damage;
};

class IAttackStrategy
{
public:
	virtual ~IAttackStrategy() {}
	// Ejecuta el ataque utilizando el Spawner para generar las balas seg�n los par�metros definidos en Params. Cada estrategia implementar� su propia l�gica para distribuir las balas en el espacio, calcular direcciones, etc.
	virtual void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) = 0;
};

class FCircleAttack : public IAttackStrategy
{
public:
	// Ejecuta un ataque circular, generando balas distribuidas en un c�rculo alrededor del origen definido en Params. La cantidad de balas, velocidad y cualquier otro par�metro relevante se toman de Params. Ver UBulletSpawnerComponent para detalles.
	void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};

class FSpiralAttack : public IAttackStrategy
{
public:
	// Ejecuta un ataque en espiral, generando balas que se distribuyen en una espiral alrededor del origen definido en Params. La cantidad de balas, velocidad, rotaci�n inicial y cualquier otro par�metro relevante se toman de Params. Ver UBulletSpawnerComponent para detalles.
	void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};

class FSphereAttack : public IAttackStrategy
{
public:
	// Ejecuta un ataque esf�rico, generando balas distribuidas en todas las direcciones alrededor del origen definido en Params.
	// Se utiliza un algoritmo tipo "Fibonacci sphere" para distribuir `Count` puntos uniformemente sobre la superficie de una esfera.
	void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};

class FBurstAttack : public IAttackStrategy
{
public:
	// Ejecuta un ataque de r�faga, generando un n�mero definido de balas en r�pida sucesi�n desde el origen definido en Params. La cantidad de balas, velocidad, intervalo entre balas y cualquier otro par�metro relevante se toman de Params. Ver UBulletSpawnerComponent para detalles.
	void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};