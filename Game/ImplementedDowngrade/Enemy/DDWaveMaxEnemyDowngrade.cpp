// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDWaveMaxEnemyDowngrade.h"

void UDDWaveMaxEnemyDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDEnemySpawner>()) {
		ADDEnemySpawner* Spawner = Cast<ADDEnemySpawner>(Target);
		Spawner->SetMaxEnemySpawnMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
