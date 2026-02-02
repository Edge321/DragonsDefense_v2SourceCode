// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerMaxHealthDowngrade.h"

void UDDPlayerMaxHealthDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetMaxHealthMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
