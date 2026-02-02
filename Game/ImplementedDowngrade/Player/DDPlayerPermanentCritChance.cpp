// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerPermanentCritChance.h"

void UDDPlayerPermanentCritChance::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->UpdateCritProjChance(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
