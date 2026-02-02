// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDPenetrativeProjectile.h"
#include "DDPlayerProjectile.generated.h"

class ALivingActor;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDPlayerProjectile : public ADDPenetrativeProjectile
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

	//How much damage a critical projectile does
	UPROPERTY(EditAnywhere, Category = "Stats")
	float CritMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Crit")
	UMaterialInterface* CriticalMaterial;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float SizeDecreaseFactor = 0.5f;

	virtual void HandleHit(ALivingActor* Living) override;

public:

	UFUNCTION(BlueprintCallable)
	void SetCritProjectileChance(const float CritChance);
	//Adjusts projectile to be the passed in size for all directions (X,Y,Z)
	UFUNCTION(BlueprintCallable)
	void SetProjectileSize(const float Size);
	//Call this after modifying the critical chance to update the variable
	UFUNCTION(BlueprintCallable)
	void CalculateCritChance();

	const FVector GetProjectileSize() const;

private:

	//Chance of shooting a critical projectile
	float CritProjectileChance = 1.0f;

	bool bIsACriticalProjectile = false;

	const FVector BoxSize = FVector(25, 25, 400);
};
