#include "Buffs/HealthPowerUp.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Buffs/HealthBonus.h"

AHealthPowerUp::AHealthPowerUp()
{
	BuffClass = UHealthBonus::StaticClass();
	BuffMagnitude = 50.0f;
	BuffDuration = 5.0f;


	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);
	//niagara
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraAsset(TEXT("NiagaraSystem'/Game/MixedVFX/Particles/Mix/NS_HealingAura.NS_HealingAura'"));
	if (NiagaraAsset.Succeeded())
	{
		NiagaraSystem = NiagaraAsset.Object;

		
		NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->SetupAttachment(RootComponent);
		NiagaraComponent->bAutoActivate = true;
	}
}