// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDSmartDebuffAreaProjectile.h"
//My classes
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"

void ADDSmartDebuffAreaProjectile::AddDebuffPair(ALivingActor* Living)
{
	if (Living) {
		if (Living->IsA<ADDPlayer>()) {
			Living->UpdateHealth(PlayerDamage);
			//Have to add Player to PairArray cus otherwise it will repeat the damage
			FDebuffPair DebuffPair;
			DebuffPair.Living = Living;
			PairArray.Add(DebuffPair);
		}
		else {
			Super::AddDebuffPair(Living);
		}
	}
}

void ADDSmartDebuffAreaProjectile::SetPlayerDamage(const float NewPlayerDamage)
{
	PlayerDamage = NewPlayerDamage;
}
