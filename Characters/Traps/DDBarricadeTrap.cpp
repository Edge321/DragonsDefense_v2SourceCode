// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDBarricadeTrap.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

void ADDBarricadeTrap::HandleBeginOverlap(AActor* Actor)
{
	if (Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		Enemy->ForceAttack(this);
	}
}

void ADDBarricadeTrap::ConvertToEvil()
{
	//Barricade cannot be evil!
}
