// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTimeBasedDebuffAreaBase.h"
//My classes
#include "DragonsDefense_v2/Game/Debuffs/DDTimeBasedDebuffBase.h"

void ADDTimeBasedDebuffAreaBase::AddDebuffPair(ALivingActor* Living)
{
	FDebuffPair DebuffPair;

	if (Living) {
		if (DebuffClass->IsChildOf<UDDTimeBasedDebuffBase>()) {
			UDDTimeBasedDebuffBase* Debuff = NewObject<UDDTimeBasedDebuffBase>(this, DebuffClass);
			Debuff->SetDebuffAmount(DebuffAmount);
			Debuff->SetDebuffLength(DebuffLength);
			Living->AddDebuff(Debuff);

			DebuffPair.Debuff = MakeWeakObjectPtr(Debuff);
			DebuffPair.Living = MakeWeakObjectPtr(Living);
			PairArray.Add(DebuffPair);
		}
		else {
			UE_LOG(LogTemp, Fatal, TEXT("Incorrect class used. Should be class of UDDTimeBasedDebuffBase"))
		}
	}
}
