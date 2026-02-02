// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemyLowSpawnDowngrade.h"

void UDDEnemyLowSpawnDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDEnemySpawner>()) {
		ADDEnemySpawner* Spawner = Cast<ADDEnemySpawner>(Target);
		Spawner->SetLowSpawnIntervalMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
