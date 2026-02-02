// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerPermanentDamageDowngrade.h"

void UDDPlayerPermanentDamageDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->UpdateDamage(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
