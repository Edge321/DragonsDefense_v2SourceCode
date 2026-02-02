// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableAttackRadiusDowngrade.h"

void UDDPlaceableAttackRadiusDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDPlaceableManager>()) {
		ADDPlaceableManager* Manager = Cast<ADDPlaceableManager>(Target);
		Manager->SetAttackRadiusMod(DowngradeAmount);
	}
	else {
		PrintErrorLog();
	}
}
