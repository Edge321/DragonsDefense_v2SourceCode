// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceablePriceDowngrade.h"

void UDDPlaceablePriceDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlaceableManager>()) {
		ADDPlaceableManager* Manager = Cast<ADDPlaceableManager>(Target);
		Manager->SetPriceMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
