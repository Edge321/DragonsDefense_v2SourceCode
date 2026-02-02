// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemySoulDowngrade.h"

void UDDEnemySoulDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDEnemySpawner>()) {
		ADDEnemySpawner* Spawner = Cast<ADDEnemySpawner>(Target);
		Spawner->SetSoulValueMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
