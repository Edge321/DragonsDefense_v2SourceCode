// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTickBasedDebuffAreaBase.h"
//My classes
#include "DragonsDefense_v2/Game/Debuffs/DDTickBasedDebuffBase.h"

void ADDTickBasedDebuffAreaBase::AddDebuffPair(ALivingActor* Living)
{
	FDebuffPair DebuffPair;

	if (Living) {
		if (DebuffClass->IsChildOf<UDDTickBasedDebuffBase>()) {
			UDDTickBasedDebuffBase* Debuff = NewObject<UDDTickBasedDebuffBase>(this, DebuffClass);
			Debuff->SetDebuffAmount(DebuffAmount);
			Debuff->SetTickInterval(DebuffTickInterval);
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
