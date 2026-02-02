// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableDamageDowngrade.h"

void UDDPlaceableDamageDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlaceableManager>()) {
		ADDPlaceableManager* Manager = Cast<ADDPlaceableManager>(Target);
		Manager->SetDamageMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
