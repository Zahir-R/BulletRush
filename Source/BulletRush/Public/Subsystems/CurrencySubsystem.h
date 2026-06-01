#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CurrencySubsystem.generated.h"

UCLASS()
class BULLETRUSH_API UCurrencySubsystem
	: public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:

	int Currency;

public:

	virtual void Initialize(
		FSubsystemCollectionBase& Collection
	) override;

	void AddCurrency(int Amount);

	bool SpendCurrency(int Amount);

	int GetCurrency() const;
};