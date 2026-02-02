// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDebuffProjectile.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Game/Debuffs/DDTimeBasedDebuffBase.h"

void ADDDebuffProjectile::HandleHit(ALivingActor* Living)
{
	Living->UpdateHealth(ProjectileDamage);

	if (Living && Living->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Living);
		UDDTimeBasedDebuffBase* Debuff = NewObject<UDDTimeBasedDebuffBase>(this, DebuffClass);
		if (Debuff) {
			Debuff->SetDebuffAmount(DebuffAmount);
			Debuff->SetDebuffLength(DebuffLength);
			Enemy->AddDebuff(Debuff);
		}
	}

	DestroySelf();
}
