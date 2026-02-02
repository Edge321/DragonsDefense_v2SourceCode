// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerProjectileSizeDowngrade.h"

void UDDPlayerProjectileSizeDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetProjectileSizeMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
