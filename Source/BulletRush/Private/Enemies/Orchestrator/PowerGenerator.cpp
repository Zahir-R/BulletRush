// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/PowerGenerator.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Components/HealthComponent.h"
#include "UI/EnemyHealthBarWidget.h"
#include "Kismet/GameplayStatics.h"

APowerGenerator::APowerGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->MaxHealth = 500.0f;

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(120.0f, 20.0f));
	HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Tags.Add(FName("Generador"));
	Tags.Add("Enemigo");
}

void APowerGenerator::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->CurrentHealth = HealthComp->MaxHealth;
	HealthComp->OnDeath.AddDynamic(this, &APowerGenerator::OnGeneratorDestroyed);

	HealthComp->OnHealthChanged.AddDynamic(this, &APowerGenerator::OnHealthUpdated);
	OnHealthUpdated(HealthComp->CurrentHealth);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetWidgetClass(UEnemyHealthBarWidget::StaticClass());
	}
}

float APowerGenerator::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HealthComp)
	{
		return HealthComp->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	return 0.0f;
}

void APowerGenerator::OnGeneratorDestroyed()
{
	// Reportamos la destrucción a la Fachada
	if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GM->LevelFacade)
		{
			GM->LevelFacade->ReportGeneratorDestroyed();
		}
	}
	Destroy();
}

void APowerGenerator::OnHealthUpdated(float NewHealth)
{
	if (!HealthBarWidget) return;

	UEnemyHealthBarWidget* Widget = Cast<UEnemyHealthBarWidget>(HealthBarWidget->GetWidget());
	if (Widget)
	{
		Widget->SetHealthPercent(NewHealth / HealthComp->MaxHealth);

		if (IsBoss())
		{
			Widget->SetIsBoss(true);
			Widget->SetBossName(GetBossDisplayName());
			Widget->SetHealthBarColor(GetHealthBarColor());
			Widget->SetHealthBarColorLow(GetHealthBarColorLow());
		}

		HealthBarWidget->SetDrawSize(GetHealthBarSize());
		HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, GetHealthBarVerticalOffset()));
	}
}

FLinearColor APowerGenerator::GetHealthBarColor() const
{
	return FLinearColor(0.f, 0.8f, 0.2f, 1.f);
}

FLinearColor APowerGenerator::GetHealthBarColorLow() const
{
	return FLinearColor(1.f, 0.2f, 0.2f, 1.f);
}
