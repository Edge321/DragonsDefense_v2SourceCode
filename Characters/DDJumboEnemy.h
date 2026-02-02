// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DDJumboEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDJumboEnemy : public AEnemy
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|EnemiesToSpawn")
	TArray<TSubclassOf<AEnemy>> EnemyExplodies;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|EnemiesToSpawn")
	int32 LowEnemySpawn = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|EnemiesToSpawn")
	int32 HighEnemySpawn = 5;

	void OnDeath() override;

public:

	virtual void Tick(float DeltaTime) override;

private:
	
	void Explode() const;
};
