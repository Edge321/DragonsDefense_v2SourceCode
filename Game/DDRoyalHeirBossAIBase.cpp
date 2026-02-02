// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDRoyalHeirBossAIBase.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"

void UDDRoyalHeirBossAIBase::DoBossAI(ADDBossEnemyBase* Boss)
{
	//Do nothing, everything already happens in the BossEnemyBase
}

void UDDRoyalHeirBossAIBase::DoBossDeath(ADDBossEnemyBase* Boss)
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(Boss->GetWorld()->GetAuthGameMode());
	if (GameMode) {
		ADDEnemySpawner& Spawner = GameMode->GetEnemySpawner();
		TArray<TSubclassOf<AEnemy>> EnemyClassArray;
		EnemyClassArray.Add(Boss->GetEnemyClassSpawn());
		Spawner.SpawnEnemies(EnemyClassArray, Boss->GetEnemySpawnAmount(), Boss->GetActorLocation());
	}
}
