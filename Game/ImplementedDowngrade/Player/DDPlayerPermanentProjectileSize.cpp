// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerPermanentProjectileSize.h"

void UDDPlayerPermanentProjectileSize::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->UpdateProjectileSize(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
