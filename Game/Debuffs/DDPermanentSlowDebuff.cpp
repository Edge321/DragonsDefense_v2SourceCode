// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDPermanentSlowDebuff.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"

void UDDPermanentSlowDebuff::ApplyDebuff(ALivingActor* Living)
{
	AEnemy* Enemy = Cast<AEnemy>(Living);
	if (Enemy) {
		float TargetSpeed = Enemy->GetMovementSpeed();
		Enemy->SetMovementSpeed(TargetSpeed * DebuffAmount);
	}
	else {
		PrintWrongActorError();
	}
}

void UDDPermanentSlowDebuff::RemoveDebuff()
{
	//Do nothing, its a permanent debuff
}
