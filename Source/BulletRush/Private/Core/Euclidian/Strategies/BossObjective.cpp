#include "Core/Euclidian/Strategies/BossObjective.h"
#include "Enemies/Euclidian/Tesseriel.h"

void UBossObjective::Initialize(
    AEuclidianGameMode* GameMode)
{
    bBossKilled = false;
}

void UBossObjective::OnEnemyKilled(
    AEnemyBase* Enemy)
{
    if (Cast<ATesseriel>(Enemy))
    {
        bBossKilled = true;
    }
}

bool UBossObjective::IsCompleted() const
{
    return bBossKilled;
}