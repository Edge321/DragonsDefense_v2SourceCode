// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDamageTickTimeDebuff.h"
//My classes
#include "DragonsDefense_v2/Game/Debuffs/DDDamageTickDebuff.h"

void UDDDamageTickTimeDebuff::ApplyDebuff(ALivingActor* Living)
{
	UDDTickTimeBasedDebuffBase::ApplyDebuff(Living);

	if (Target.IsValid()) {
		Living->UpdateHealth(DebuffAmount);
	}
}

void UDDDamageTickTimeDebuff::UpdateDebuff(float DeltaTime)
{
	TotalTimePassed += DeltaTime;
	while (TotalTimePassed >= DebuffTickInterval) {
		if (Target.IsValid()) {
			Target->UpdateHealth(DebuffAmount);
		}
		TotalTimePassed -= DebuffTickInterval;
	}
}
