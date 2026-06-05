// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Euclidian/Builders/TessarielBuilder.h"
#include "DefaultTesserielBuilder.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UDefaultTesserielBuilder : public UTesserielBuilder
{
	GENERATED_BODY()

public:
	virtual void BuildBoss(
		ATesseriel* Boss) override;
};
