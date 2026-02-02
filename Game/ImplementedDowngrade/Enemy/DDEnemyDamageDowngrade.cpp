// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemyDamageDowngrade.h"

void UDDEnemyDamageDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDEnemySpawner>()) {
		ADDEnemySpawner* Spawner = Cast<ADDEnemySpawner>(Target);
		Spawner->SetDamageMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
