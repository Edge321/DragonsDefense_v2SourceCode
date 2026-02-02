// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerPermanentShootSpeedDown.h"

void UDDPlayerPermanentShootSpeedDown::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->UpdateShootSpeed(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
