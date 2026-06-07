#include "Combat/AttackPatterns/SphereAttack.h"
#include "Components/BulletSpawnerComponent.h"

void USphereAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner) return;

	// Esfera de Fibonacci para distribución casi uniforme en la superficie de la esfera
	const float GoldenAngle = PI * (3.0f - FMath::Sqrt(5.0f));

	for (int32 i = 0; i < Params.Count; ++i)
	{
		float t = (Params.Count > 1) ? (float)i / (float)(Params.Count - 1) : 0.5f;
		float y = 1.0f - 2.0f * t;
		float radius = FMath::Sqrt(FMath::Max(0.0f, 1.0f - y * y));
		float theta = GoldenAngle * i;

		float x = FMath::Cos(theta) * radius;
		float z = FMath::Sin(theta) * radius;

		FVector Direction(x, y, z);
		Direction = Direction.GetSafeNormal();

		Spawner->InternalSpawn(Params.Origin, Direction, Params.Speed, Params.Damage, Params.Scale);
	}
}