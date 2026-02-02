// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTimeBasedDebuffBase.h"

void UDDTimeBasedDebuffBase::SetDebuffLength(const float NewDebuffLength)
{
	DebuffLength = NewDebuffLength;
}

void UDDTimeBasedDebuffBase::StartDebuff()
{
	if (Target.IsValid()) {
		UWorld* World = Target->GetWorld();
		if (World) {
			World->GetTimerManager().SetTimer(DebuffTimer, this, &UDDDebuffBase::RemoveDebuff, DebuffLength);
		}
	}
}
