#include "Subsystems/CurrencySubsystem.h"

void UCurrencySubsystem::Initialize(
	FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Currency = 0;
}

void UCurrencySubsystem::AddCurrency(int Amount)
{
	Currency += Amount;
}

bool UCurrencySubsystem::SpendCurrency(int Amount)
{
	if (Currency < Amount)
	{
		return false;
	}

	Currency -= Amount;

	return true;
}

int UCurrencySubsystem::GetCurrency() const
{
	return Currency;
}