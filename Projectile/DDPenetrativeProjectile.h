// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DDPenetrativeProjectile.generated.h"

class ALivingActor;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDPenetrativeProjectile : public ADDProjectile
{
	GENERATED_BODY()
	
protected:

	//How many objects can this projectile penetrate through. <=1 for no penetration
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Penetration")
	int32 MaxObjectsToPenetrate = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Penetration")
	float DamageAfterPenetrationModifier = 0.75f;

	virtual void HandleHit(ALivingActor* Living) override;

	TWeakObjectPtr<ALivingActor> RecentlyHitActor;
public:

	UFUNCTION(BlueprintCallable)
	void SetMaxObjectsToPenetrate(const int32 MaxObjects);
	
	const int32 GetMaxObjectsToPenetrate() const;

};
