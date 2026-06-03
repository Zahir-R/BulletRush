// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TesserielStrategy.generated.h"

class ATesseriel;

UCLASS(Abstract)
class BULLETRUSH_API UTesserielStrategy : public UObject
{
	GENERATED_BODY()

public:

	virtual void Enter(ATesseriel* Boss) {}
	virtual void Exit(ATesseriel* Boss) {}

	virtual bool CanTakeDamage() const
	{
		return false;
	}
};