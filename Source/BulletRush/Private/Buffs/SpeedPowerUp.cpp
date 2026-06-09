#include "Buffs/SpeedPowerUp.h"
#include "Buffs/SpeedBoost.h"

ASpeedPowerUp::ASpeedPowerUp()
{
	BuffClass = USpeedBoost::StaticClass();
	BuffMagnitude = 2.0f;
	BuffDuration = 5.0f;

	//niagara
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraAsset(TEXT("NiagaraSystem'/Game/MixedVFX/Particles/Mix/NS_ElectricField.NS_ElectricField'"));
	if (NiagaraAsset.Succeeded())
	{
		NiagaraSystem = NiagaraAsset.Object;


		NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->SetupAttachment(RootComponent);
		NiagaraComponent->bAutoActivate = true;
	}
}