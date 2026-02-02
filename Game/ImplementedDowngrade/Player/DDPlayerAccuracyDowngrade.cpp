// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerAccuracyDowngrade.h"

void UDDPlayerAccuracyDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetInaccuracyMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
