// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDSoulPermanentDowngrade.h"

void UDDSoulPermanentDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDGameModeBase>()) {
		ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(Target);
		GameMode->UpdateSouls(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
