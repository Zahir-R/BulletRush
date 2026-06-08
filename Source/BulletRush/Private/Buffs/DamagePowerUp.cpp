#include "Buffs/DamagePowerUp.h"
#include "Buffs/DoubleDamage.h"

ADamagePowerUp::ADamagePowerUp()
{
	PrimaryActorTick.bCanEverTick = true;
	BuffClass = UDoubleDamage::StaticClass();
	BuffMagnitude = 0.0f;
	BuffDuration = 5.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/Assets/PowerUps/BlueMushroom/Blue_Mushroom.Blue_Mushroom'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HealthMat(TEXT("Material'/Game/Assets/PowerUps/BlueMushroom/M_Blue_Mushroom.M_Blue_Mushroom'"));
	if (HealthMat.Succeeded()) Mesh->SetMaterial(0, HealthMat.Object);
	Mesh->SetRelativeScale3D(FVector(0.60f, 0.60f, 0.60f));
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}