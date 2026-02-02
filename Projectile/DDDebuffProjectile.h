// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DDDebuffProjectile.generated.h"

class ALivingActor;
class UDDTimeBasedDebuffBase;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDDebuffProjectile : public ADDProjectile
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	TSubclassOf<UDDTimeBasedDebuffBase> DebuffClass;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffAmount = 0.9f;
	//How long debuff last in seconds
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffLength = 3.0f;

	void HandleHit(ALivingActor* Living) override;

};
