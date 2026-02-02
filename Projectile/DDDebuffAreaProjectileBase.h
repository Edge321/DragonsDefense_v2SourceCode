// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaInterface.h"
#include "DDDebuffAreaProjectileBase.generated.h"

/**
 * Projectile serves as a moving debuff area its entire lifetime
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDDebuffAreaProjectileBase : public ADDProjectile, public IDDDebuffAreaInterface
{
	GENERATED_BODY()
	
public:

	ADDDebuffAreaProjectileBase();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	TSubclassOf<UDDDebuffBase> DebuffClass;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffTickInterval = 1.0f;

	virtual void OverlapAreaBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OverlapAreaEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual void HandleHit(ALivingActor* Living) override;
	virtual void AddDebuffPair(ALivingActor* Living) override;
	virtual void DestroySelf() override;

public:

	virtual void SetCollisionChannelToIgnore(ECollisionChannel Channel) override;

};
