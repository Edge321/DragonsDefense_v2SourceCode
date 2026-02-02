// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDebuffAreaInterface.h"
//My classes
#include "DragonsDefense_v2/Game/Debuffs/DDTimeBasedDebuffBase.h"

void IDDDebuffAreaInterface::RemoveDebuffPair(ALivingActor* Living)
{
	for (FDebuffPair Pair : PairArray) {
		if (!Pair.Living.IsValid()) {
			PairArray.RemoveSwap(Pair);
			return;
		}
		else if (Pair.Living == Living) {
			Pair.Debuff->RemoveDebuff();
			PairArray.RemoveSwap(Pair);
			return;
		}
	}
}

void IDDDebuffAreaInterface::RemoveDebuffPair(UDDDebuffBase* Debuff)
{
	for (FDebuffPair Pair : PairArray) {
		if (!Pair.Debuff.IsValid()) {
			PairArray.RemoveSwap(Pair);
			return;
		}
		else if (Pair.Debuff == Debuff) {
			Pair.Debuff->RemoveDebuff();
			PairArray.RemoveSwap(Pair);
			return;
		}
	}
}

void IDDDebuffAreaInterface::RemoveDebuffs()
{
	for (FDebuffPair Pair : PairArray) {
		if (Pair.Debuff.IsValid() && !Pair.Debuff->IsA<UDDTimeBasedDebuffBase>()) {
			Pair.Debuff->RemoveDebuff();
		}
	}

	PairArray.Empty();
}
