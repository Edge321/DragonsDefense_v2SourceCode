// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerPermanentProjectileSpeed.h"

void UDDPlayerPermanentProjectileSpeed::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->UpdateProjectileSpeed(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
