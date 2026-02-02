// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTickTimeBasedDebuffBase.h"

void UDDTickTimeBasedDebuffBase::ApplyDebuff(ALivingActor* Living)
{
	Target = MakeWeakObjectPtr(Living);
	StartTimer();
}

void UDDTickTimeBasedDebuffBase::RemoveDebuff()
{
	if (Target.IsValid()) {
		OnRemoveDebuff.ExecuteIfBound(this);
	}
}

void UDDTickTimeBasedDebuffBase::SetDebuffLength(const float NewDebuffLength)
{
	DebuffLength = NewDebuffLength;
}

void UDDTickTimeBasedDebuffBase::StartTimer()
{
	if (Target.IsValid()) {
		UWorld* World = Target->GetWorld();
		if (World) {
			World->GetTimerManager().SetTimer(DebuffTimer, this, &UDDDebuffBase::RemoveDebuff, DebuffLength);
		}
	}
}
