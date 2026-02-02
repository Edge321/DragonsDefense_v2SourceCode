// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDMagicMineTrap.h"
//My classes
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"
#include "DragonsDefense_v2/Characters/Enemy.h"

void ADDMagicMineTrap::HandleBeginOverlap(AActor* Actor)
{
	if (!bIsAboutToExplode && Actor->IsA<AEnemy>()) {
		bIsAboutToExplode = true;
		AddChannelHurtByExplosion(ECC_EnemyChannel);
		OnExplode();
	}
}

void ADDMagicMineTrap::ConvertToEvil()
{
	if (!bIsAboutToExplode) {
		Super::ConvertToEvil();
	}
}
