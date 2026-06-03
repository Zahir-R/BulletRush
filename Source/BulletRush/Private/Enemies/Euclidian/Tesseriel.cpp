// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/Euclidian/Tesseriel.h"
#include "Enemies/Euclidian/Builders/DefaultTesserielBuilder.h"
#include "Enemies/Euclidian/TesserielComponents/TesserielImmuneStrategy.h"
#include "Enemies/Euclidian/TesserielComponents/TesserielVulnerableStrategy.h"
#include "Enemies/Euclidian/TesserielComponents/TesserielDeadStrategy.h"

ATesseriel::ATesseriel()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATesseriel::BeginPlay()
{
	Super::BeginPlay();

	ImmuneStrategy =
		NewObject<UTesserielImmuneStrategy>(this);

	VulnerableStrategy =
		NewObject<UTesserielVulnerableStrategy>(this);

	DeadStrategy =
		NewObject<UTesserielDeadStrategy>(this);

	CurrentStrategy = ImmuneStrategy;

	Builder =
		NewObject<UDefaultTesserielBuilder>(this);

	Builder->BuildBoss(this);
}