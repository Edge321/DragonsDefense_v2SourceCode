// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDamageTickDebuff.h"

void UDDDamageTickDebuff::ApplyDebuff(ALivingActor* Living)
{
	Target = MakeWeakObjectPtr(Living);
	if (Target.IsValid()) {
		Living->UpdateHealth(DebuffAmount);
	}
}

void UDDDamageTickDebuff::RemoveDebuff()
{
	if (Target.IsValid()) {
		OnRemoveDebuff.ExecuteIfBound(this);
	}
}

void UDDDamageTickDebuff::UpdateDebuff(float DeltaTime)
{
	TotalTimePassed += DeltaTime;
	while (TotalTimePassed >= DebuffTickInterval) {
		if (Target.IsValid()) {
			Target->UpdateHealth(DebuffAmount);
		}
		TotalTimePassed -= DebuffTickInterval;
	}
}
