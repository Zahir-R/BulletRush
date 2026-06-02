#include "Core/PlayerHealthPublisher.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerHealthPublisher::APlayerHealthPublisher()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlayerHealthPublisher::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		UHealthComponent* HealthComp = PlayerPawn->FindComponentByClass<UHealthComponent>();
		if (HealthComp)
		{
			InitialHealth = HealthComp->CurrentHealth;
			CurrentHealth = InitialHealth;
			HealthComp->OnHealthChanged.AddDynamic(this, &APlayerHealthPublisher::OnPlayerHealthChanged);
		}
	}
}

void APlayerHealthPublisher::OnPlayerHealthChanged(float NewHealth)
{
	CurrentHealth = NewHealth;
	if (NewHealth < InitialHealth)
	{
		NotifySubscribers();
		InitialHealth = -1.f;
	}
}
