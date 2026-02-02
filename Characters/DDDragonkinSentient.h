// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"
#include "DDDragonkinSentient.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDDragonkinSentient : public ADDSentientPlaceable
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	void InitializeProjectile(ADDProjectile* Proj, const float CustomDamage) const override;

private:

	void InheritPlayerStats();
	void SetHealth(const float NewHealth);
	void SetAttackSpeed(const float NewAttackSpeed);
	void SetProjectileSpeed(const float NewProjectileSpeed);
	void SetDamage(const float NewDamage);
	void SetPenetration(const int32 NewPenetration);
	void SetProjectileSize(const float NewProjectileSizeMod);

	int32 Penetration = 1;
	float ProjectileSpeed;
	float ProjectileSize;

};
