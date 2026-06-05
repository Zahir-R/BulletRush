#include "Combat/AttackPatterns/SpiralAttack.h"
#include "Components/BulletSpawnerComponent.h"

void USpiralAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner) return;

	for (int32 i = 0; i < Params.Count; ++i)
	{
		float Angle = (PI * 2.0f / Params.Count) * i + FMath::DegreesToRadians(Params.SpecialParam);
		FVector Direction(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
		Direction = Direction.GetSafeNormal();

		float BulletSpeed = Params.Speed * (1.0f + (i / (float)FMath::Max(1, Params.Count)) * 1.5f);

		Spawner->InternalSpawn(Params.Origin, Direction, BulletSpeed, Params.Damage, Params.Scale);
	}
}