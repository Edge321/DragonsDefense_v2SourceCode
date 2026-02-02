// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDSlowDebuff.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Characters/Bosses/DDBossEnemyBase.h"

void UDDSlowDebuff::ApplyDebuff(ALivingActor* Living)
{
	Target = MakeWeakObjectPtr(Living);
	if (Living->IsA<AEnemy>()) {
		EnemyRef = MakeWeakObjectPtr(Cast<AEnemy>(Living));
	}
	else {
		PrintWrongActorError();
	}
	if (EnemyRef.IsValid()) {
		const float TargetSpeed = EnemyRef->GetMovementSpeed();
		DebuffAmount = 1.0f - ((1.0f - DebuffAmount) * (1.0f - EnemyRef->GetSlowDebuffResistance()));
		EnemyRef->SetMovementSpeed(TargetSpeed * DebuffAmount);
	}
}

void UDDSlowDebuff::RemoveDebuff()
{
	if (EnemyRef.IsValid()) {
		float TargetSpeed = EnemyRef->GetMovementSpeed();
		float InverseDebuff = 1.0f + (1.0f - DebuffAmount);
		EnemyRef->SetMovementSpeed(TargetSpeed * InverseDebuff);
		OnRemoveDebuff.ExecuteIfBound(this);
	}
}
