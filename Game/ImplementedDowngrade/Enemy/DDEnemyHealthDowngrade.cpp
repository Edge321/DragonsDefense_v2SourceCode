// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemyHealthDowngrade.h"

void UDDEnemyHealthDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDEnemySpawner>()) {
		ADDEnemySpawner* Spawner = Cast<ADDEnemySpawner>(Target);
		Spawner->SetHealthMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
