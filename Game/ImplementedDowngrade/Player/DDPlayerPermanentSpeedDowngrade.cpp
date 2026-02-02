// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerPermanentSpeedDowngrade.h"

void UDDPlayerPermanentSpeedDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->UpdateMovementSpeed(DowngradeAmount);
	}
}
