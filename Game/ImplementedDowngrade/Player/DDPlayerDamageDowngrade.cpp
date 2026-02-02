// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerDamageDowngrade.h"

void UDDPlayerDamageDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetDamageMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
