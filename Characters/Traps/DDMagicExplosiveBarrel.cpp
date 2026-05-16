// Copyright (c) 2026, Edge Cope Corp. All rights reserved


#include "DDMagicExplosiveBarrel.h"
//My classes
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

void ADDMagicExplosiveBarrel::BeginPlay() {
	Super::BeginPlay();

	AddChannelHurtByExplosion(ECC_EnemyChannel);
	OnExplode();
}
