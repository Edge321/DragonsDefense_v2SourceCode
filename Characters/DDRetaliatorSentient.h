// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"
#include "DDRetaliatorSentient.generated.h"

class ADDProjectile;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDRetaliatorSentient : public ADDSentientPlaceable
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Retaliate")
	float RetaliateDamageMultiplier = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Retaliate")
	float RetaliateBossDamage = 4.0f;

public:

	void HandleBeingHit(ADDProjectile* ProjectileHitBy);
	
private:

	void SetReturnProjectileStats(ADDProjectile* RetaliateProj, ADDProjectile* ProjectileHitBy);

};
