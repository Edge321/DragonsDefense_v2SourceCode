// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerSpeedDowngrade.h"

void UDDPlayerSpeedDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetMovementSpeedMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
