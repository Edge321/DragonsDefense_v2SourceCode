// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Clickables/DDClickableBase.h"
#include "DDExplosiveBarrel.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDExplosiveBarrel : public ADDClickableBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Stats|Explosion")
	float ExplosionRadius = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Stats|Explosion")
	float TimeToExplode = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Stats|Explosion")
	float ExplosionDamage = 10.0f;

private:

	void Explode();

	FTimerHandle ExplodeTimer;

};
