// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerProjectileSpeedDowngrade.h"

void UDDPlayerProjectileSpeedDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetProjectileSpeedMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
