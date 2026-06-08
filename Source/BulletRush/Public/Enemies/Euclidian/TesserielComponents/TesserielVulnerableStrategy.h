// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Euclidian/TesserielComponents/TesserielStrategy.h"
#include "TesserielVulnerableStrategy.generated.h"

UCLASS()
class BULLETRUSH_API UTesserielVulnerableStrategy
	: public UTesserielStrategy
{
	GENERATED_BODY()

public:

	virtual bool CanTakeDamage() const override;
};
