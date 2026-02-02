// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DDExplosiveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDExplosiveProjectile : public ADDProjectile
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Explosion")
	float ExplosionRadius = 300.0f;

	void HandleHit(ALivingActor* Living) override;

private:

	void Explode();

};
