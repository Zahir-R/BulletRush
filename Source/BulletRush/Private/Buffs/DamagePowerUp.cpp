#include "Buffs/DamagePowerUp.h"
#include "Buffs/Buff_DoubleDamage.h"

ADamagePowerUp::ADamagePowerUp()
{
	PrimaryActorTick.bCanEverTick = true;


	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Torus.Shape_Torus'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);
}