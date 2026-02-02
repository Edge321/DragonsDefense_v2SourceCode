// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DDSpawnDebuffAreaProjectile.generated.h"

class ADDDebuffAreaBase;
class ALivingActor;

/**
 * Spawns a DebuffArea Actor when the projectile hits a valid Actor
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDSpawnDebuffAreaProjectile : public ADDProjectile
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|DebuffArea")
	TSubclassOf<ADDDebuffAreaBase> DebuffAreaClass;

	void HandleHit(ALivingActor* Living) override;
	
};
