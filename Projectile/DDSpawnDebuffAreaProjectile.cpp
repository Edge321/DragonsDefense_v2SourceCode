// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDSpawnDebuffAreaProjectile.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/Managers/DDDebuffAreaManager.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaBase.h"
#include "DragonsDefense_v2/Characters/LivingActor.h"

void ADDSpawnDebuffAreaProjectile::HandleHit(ALivingActor* Living)
{
	if (Living) {
		ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode) {
			ADDDebuffAreaBase* DebuffArea = GameMode->GetDebuffAreaManager().SpawnDebuffArea(DebuffAreaClass, GetActorLocation(), FRotator::ZeroRotator, true);
			//DebuffArea->SetDebuffAmount()
			for (ECollisionChannel Channel : ChannelsIgnored) {
				DebuffArea->SetCollisionChannelToIgnore(Channel);
			}
		}

		DestroySelf();
	}
}
