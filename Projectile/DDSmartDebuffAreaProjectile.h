// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDDebuffAreaProjectileBase.h"
#include "DDSmartDebuffAreaProjectile.generated.h"

/**
 * Is able to differentiate what is a player and what isn't a player
 * Has two different types of damages for the player and not player
 * So, barely a smart projectile really
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDSmartDebuffAreaProjectile : public ADDDebuffAreaProjectileBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float PlayerDamage = -8.0f;

	virtual void AddDebuffPair(ALivingActor* Living) override;

public:
	//Set damage for the projectile that is specifically meant for the player
	void SetPlayerDamage(const float NewPlayerDamage);

};
