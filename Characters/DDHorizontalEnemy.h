// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "Components/TimelineComponent.h"
#include "DDHorizontalEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDHorizontalEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADDHorizontalEnemy();

protected:

	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	int32 BounceAmount = 1;
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	bool bOverrideBounceSpawn = false;
	//Absolute location on X-axis
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	float LowerBounceSpawn = 0;
	//Absolute location on X-axis
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	float HighBounceSpawn = 0;

	void OnNoBounceBack();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Sets location on X-axis only
	void SetBounceLocation(const float LocationX);
	const bool GetOverrideBounceSpawn() const;
	void GetBounceIntervalSpawn(float& LowBounce, float& HighBounce) const;

private:
	//Don't know why but interpolation to 0 velocity fixes teleportation bug
	void ResetPawnVelocity(float DeltaTime);

	bool bIsBounced = false;

	//Variables for resetting velocity
	FVector CurrentValue;
	FVector TargetValue = FVector::Zero();
	float InterpolationDuration = 0.15f;
	float ElapsedTime = 0.0f;

};
