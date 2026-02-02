// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerPermanentMaxHealthDown.h"

void UDDPlayerPermanentMaxHealthDown::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->UpdateMaxHealth(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
