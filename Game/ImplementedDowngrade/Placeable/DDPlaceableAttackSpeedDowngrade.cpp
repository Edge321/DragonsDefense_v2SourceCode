// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableAttackSpeedDowngrade.h"

void UDDPlaceableAttackSpeedDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlaceableManager>()) {
		ADDPlaceableManager* Manager = Cast<ADDPlaceableManager>(Target);
		Manager->SetAttackSpeedMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
