#pragma once

#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "PlayerHealthPublisher.generated.h"

UCLASS()
class BULLETRUSH_API APlayerHealthPublisher : public APublisher
{
	GENERATED_BODY()

public:
	APlayerHealthPublisher();
	virtual void BeginPlay() override;

	void OnPlayerHealthChanged(float NewHealth);

	float GetInitialHealth() const { return InitialHealth; }
	float GetCurrentHealth() const { return CurrentHealth; }

private:
	float InitialHealth = 0.f;
	float CurrentHealth = 0.f;
};
