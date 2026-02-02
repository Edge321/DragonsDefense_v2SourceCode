// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DDShrewdEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDShrewdEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:

	void Tick(float DeltaTime) override;

	//Returns amount of innaccuracy in centimeters
	const FVector GetRandomInaccuracy() const;

protected:
	
	//How wide the zig-zag pattern is. Limited by how high frequeny is
	UPROPERTY(EditAnywhere, Category = "Stats|Movement")
	float Amplitude = 500.0f;
	//How fast they are zig-zagging
	UPROPERTY(EditAnywhere, Category = "Stats|Movement")
	float Frequency = 1.0f;
	//How innaccurate a sentient can be when getting targeted
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Shrewdness")
	float MaxInaccuracy = 100.0f;

};
