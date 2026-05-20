#include "Buffs/DamagePowerUp.h"
#include "Buffs/DoubleDamage.h"

ADamagePowerUp::ADamagePowerUp()
{
	PrimaryActorTick.bCanEverTick = true;
	BuffClass = UDoubleDamage::StaticClass();
	BuffMagnitude = 0.0f;
	BuffDuration = 5.0f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Torus.Shape_Torus'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);
}