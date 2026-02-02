// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerCritChanceDowngrade.h"

void UDDPlayerCritChanceDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlayer>()) {
		ADDPlayer* Player = Cast<ADDPlayer>(Target);
		Player->SetCritProjChanceMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
