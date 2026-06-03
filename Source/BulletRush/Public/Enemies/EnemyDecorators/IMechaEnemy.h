#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IMechaEnemy.generated.h"

// Esta clase no se modifica (requerido por UE)
UINTERFACE(MinimalAPI)
class UMechaEnemy : public UInterface
{
    GENERATED_BODY()
};

class BULLETRUSH_API IMechaEnemy
{
    GENERATED_BODY()

public:
    // Ejecuta el ataque del enemigo
    virtual void StartAttack() = 0;

    // Mata al enemigo
    virtual void Die() = 0;
};