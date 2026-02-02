// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDHorizontalEnemy.h"
#include "DDTrapperEnemy.generated.h"

class ADDGameModeBase;
class ADDClickableBase;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDTrapperEnemy : public ADDHorizontalEnemy
{
	GENERATED_BODY()
	
protected:

	void virtual BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float LowSpawnExplosiveInterval = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float HighSpawnExplosiveInterval = 7.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	TSubclassOf<ADDClickableBase> ClickableClass;

private:

	void StartSpawning();
	void SpawnExplosiveBarrel();

	FTimerHandle TimeSpawnExplosive;
	TWeakObjectPtr<ADDGameModeBase> GameModeRef;

};
