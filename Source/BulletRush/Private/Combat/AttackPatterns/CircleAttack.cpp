#include "Combat/AttackPatterns/CircleAttack.h"
#include "Components/BulletSpawnerComponent.h"

void UCircleAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner) return;

	for (int32 i = 0; i < Params.Count; ++i)
	{
		float Angle = PI * 2.0f / Params.Count * i;
		FVector Direction(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
		Direction = Direction.GetSafeNormal();

		Spawner->InternalSpawn(Params.Origin, Direction, Params.Speed, Params.Damage);
	}
}