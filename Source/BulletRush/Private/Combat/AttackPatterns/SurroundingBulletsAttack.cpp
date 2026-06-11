#include "Combat/AttackPatterns/SurroundingBulletsAttack.h"
#include "Components/BulletSpawnerComponent.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Combat/BulletBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void USurroundingBulletsAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner) return;

	UProjectilesSubsystem* Subsystem = Spawner->GetProjectilesSubsystem();
	if (!Subsystem) return;

	AActor* Owner = Spawner->GetOwner();
	if (!Owner) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(Spawner->GetWorld(), 0);
	FVector TargetCenter = PC && PC->GetPawn() ? PC->GetPawn()->GetActorLocation() : Params.Origin;

	const float Radius = Params.SpecialParam > 0.f ? Params.SpecialParam : 500.f;
	const float ConvergeDelay = 3.f;
	const float GoldenAngle = PI * (3.0f - FMath::Sqrt(5.0f));

	for (int32 i = 0; i < Params.Count; ++i)
	{
		float t = (Params.Count > 1) ? (float)i / (float)(Params.Count - 1) : 0.5f;
		float y = 1.0f - 2.0f * t;
		float r = FMath::Sqrt(FMath::Max(0.0f, 1.0f - y * y));
		float theta = GoldenAngle * i;

		float x = FMath::Cos(theta) * r;
		float z = FMath::Sin(theta) * r;

		FVector Dir(x, y, z);
		Dir = Dir.GetSafeNormal();

		FVector SpawnPos = TargetCenter + Dir * Radius;

		ABulletBase* Bullet = Subsystem->RequestBullet(SpawnPos, Dir, 0.f, false, Params.Damage, SpawnPos, Owner, Params.Scale);
		if (Bullet)
		{
			Bullet->BulletData.ConvergeCenter = TargetCenter;
			Bullet->BulletData.ConvergeDelay = ConvergeDelay;
		}
	}
}
