#pragma once

#include "CoreMinimal.h"
#include "Enemies/Common/Turret.h"
#include "RedTurret.generated.h"

UCLASS()
class BULLETRUSH_API ARedTurret : public ATurret
{
	GENERATED_BODY()

public:
	ARedTurret();

protected:
	virtual void BeginPlay() override;
};