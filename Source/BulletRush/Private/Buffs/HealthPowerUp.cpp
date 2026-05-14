#include "Buffs/HealthPowerUp.h"
#include "Buffs/Buff_HealthBoost.h"

AHealthPowerUp::AHealthPowerUp()
{
	BuffClass = UBuff_HealthBoost::StaticClass();
	BuffMagnitude = 50.0f;
	BuffDuration = 5.0f;


	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);
}