#include "Buffs/UltimatePowerUp.h"
#include "Buffs/PowerUpManager.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/EnemyBase.h"
#include "Subsystems/ProjectilesSubsystem.h"

AUltimatePowerUp::AUltimatePowerUp()
{
	Type = EPowerUpType::Ultimate;
	BuffClass = nullptr;

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AUltimatePowerUp::OnUltimateOverlap);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (CubeMesh.Succeeded()) Mesh->SetStaticMesh(CubeMesh.Object);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AUltimatePowerUp::OnUltimateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->ActorHasTag("Player")) return;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		Enemy->TakeDamage(500.0f, FDamageEvent(), nullptr, this);
	}

	UProjectilesSubsystem* ProjectilesSub = GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
	if (ProjectilesSub)
	{
		ProjectilesSub->ReturnAllActiveBullets();
	}

	if (ManagerRef.IsValid()) ManagerRef->OnPowerUpCollected(this);
	Destroy();
}
