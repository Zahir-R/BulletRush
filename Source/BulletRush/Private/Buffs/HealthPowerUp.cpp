#include "Buffs/HealthPowerUp.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Buffs/HealthBonus.h"

AHealthPowerUp::AHealthPowerUp()
{
	BuffClass = UHealthBonus::StaticClass();
	BuffMagnitude = 50.0f;
	BuffDuration = 5.0f;


	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/Assets/PowerUps/RedMushroom/Red_Mushroom.Red_Mushroom'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HealthMat(TEXT("Material'/Game/Assets/PowerUps/RedMushroom/M_Red_Mushroom.M_Red_Mushroom'"));
	if (HealthMat.Succeeded()) Mesh->SetMaterial(0, HealthMat.Object);
	Mesh->SetRelativeScale3D(FVector(0.60f, 0.60f, 0.60f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
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