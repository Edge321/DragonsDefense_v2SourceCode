// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DDDisrupterEnemy.generated.h"

class ADDCrater;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDDisrupterEnemy : public AEnemy
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Disruption")
	TSubclassOf<ADDCrater> DisruptionClass;
	UPROPERTY(EditAnywhere, Category = "Stats|Disruption")
	FVector DisruptionSize = FVector(250, 250, 5);
	UPROPERTY(EditAnywhere, Category = "Stats|Disruption")
	float AreaLimitY = 3400.0f;
	
public:

	virtual void Tick(float DeltaTime) override;

	void RandomizeDisruptionLocation();
	const FVector GetDisruptionLocation() const;
	const FVector GetDisruptionSize() const;

private:

	void PlaceDisruption();
	void OnDisappear();

	UFUNCTION()
	void WaveStartEventFunction();

	bool bIsPlacingDisruption = false;
	bool bLeavingBattlefield = false;
	bool bStartMoving = false;
	FVector DisruptionLocation = FVector(0, 0, 1);

	float LeavingTime = 10.0f;
	FTimerHandle LeavingLifeTime;

	FVector PlayerLocation = FVector::Zero();
	FVector SpawnerLocation = FVector::Zero();
	
};
