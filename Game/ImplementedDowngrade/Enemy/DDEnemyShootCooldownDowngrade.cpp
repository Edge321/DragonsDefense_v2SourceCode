// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemyShootCooldownDowngrade.h"

void UDDEnemyShootCooldownDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDEnemySpawner>()) {
		ADDEnemySpawner* Spawner = Cast<ADDEnemySpawner>(Target);
		Spawner->SetShootCooldownMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
