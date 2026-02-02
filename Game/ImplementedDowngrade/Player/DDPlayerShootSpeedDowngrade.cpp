// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerShootSpeedDowngrade.h"

void UDDPlayerShootSpeedDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetShootSpeedMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
