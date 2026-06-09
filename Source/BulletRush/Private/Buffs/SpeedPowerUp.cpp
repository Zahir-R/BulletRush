#include "Buffs/SpeedPowerUp.h"
#include "Buffs/SpeedBoost.h"

ASpeedPowerUp::ASpeedPowerUp()
{
	BuffClass = USpeedBoost::StaticClass();
	BuffMagnitude = 2.0f;
	BuffDuration = 5.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/Assets/PowerUps/GreenMushroom/Green_Mushroom.Green_Mushroom'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HealthMat(TEXT("Material'/Game/Assets/PowerUps/GreenMushroom/M_Green_Mushroom.M_Green_Mushroom'"));
	if (HealthMat.Succeeded()) Mesh->SetMaterial(0, HealthMat.Object);
	Mesh->SetRelativeScale3D(FVector(0.60f, 0.60f, 0.60f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
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