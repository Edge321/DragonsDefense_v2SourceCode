// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemyHighSpawnDowngrade.h"

void UDDEnemyHighSpawnDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDEnemySpawner>()) {
		ADDEnemySpawner* Spawner = Cast<ADDEnemySpawner>(Target);
		Spawner->SetHighSpawnIntervalMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
