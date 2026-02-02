// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTickTimeBasedDebuffAreaBase.h"
//My classes
#include "DragonsDefense_v2/Game/Debuffs/DDTickTimeBasedDebuffBase.h"

void ADDTickTimeBasedDebuffAreaBase::AddDebuffPair(ALivingActor* Living)
{
	FDebuffPair DebuffPair;

	if (Living) {
		if (DebuffClass->IsChildOf<UDDTickTimeBasedDebuffBase>()) {
			UDDTickTimeBasedDebuffBase* Debuff = NewObject<UDDTickTimeBasedDebuffBase>(this, DebuffClass);
			Debuff->SetDebuffAmount(DebuffAmount);
			Debuff->SetTickInterval(DebuffTickInterval);
			Debuff->SetDebuffLength(DebuffLength);
			Living->AddDebuff(Debuff);

			DebuffPair.Debuff = MakeWeakObjectPtr(Debuff);
			DebuffPair.Living = MakeWeakObjectPtr(Living);
			PairArray.Add(DebuffPair);
		}
		else {
			UE_LOG(LogTemp, Fatal, TEXT("Incorrect class used. Should be class of UDDTickTimeBasedDebuffBase"))
		}
	}
}
