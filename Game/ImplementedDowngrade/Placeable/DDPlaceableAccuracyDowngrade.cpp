// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableAccuracyDowngrade.h"

void UDDPlaceableAccuracyDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlaceableManager>()) {
		ADDPlaceableManager* Manager = Cast<ADDPlaceableManager>(Target);
		Manager->SetAccuracyMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
