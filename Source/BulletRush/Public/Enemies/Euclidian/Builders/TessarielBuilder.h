// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TessarielBuilder.generated.h"

class ATesseriel;

UCLASS(Abstract)
class BULLETRUSH_API UTesserielBuilder
	: public UObject
{
	GENERATED_BODY()

public:

	virtual void BuildBoss(
		ATesseriel* Boss)
		PURE_VIRTUAL(UTesserielBuilder::BuildBoss, );
};