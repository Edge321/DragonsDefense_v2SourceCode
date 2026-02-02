// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/DDBossAIBase.h"
#include "DDKingBossAI.generated.h"

class ADDEnemySpawner;
class ADDGameModeBase;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDKingBossAI : public UDDBossAIBase
{
	GENERATED_BODY()

public:

	virtual void DoBossAI(ADDBossEnemyBase* Boss) override;

	virtual void DoBossDeath(ADDBossEnemyBase* Boss) override;

private:

	enum class FBossStages : uint8 {
		InitialStage,
		DamagedStage,
		InjuredStage,
		NearDeathStage
	};

	FTimerHandle EnemySummonTimer;


	void InitializeAI(ADDBossEnemyBase* Boss);

	void UpdateHealthBossEventFunction(const float TotalHealth, const float CurrentHealth);
	void ExecuteBossStage(const float TotalHealth, const float CurrentHealth);
	void SummonEnemy();
	void AddPermanentSlowDebuff(const float DebuffAmount);

	bool bHasInitialized = false;
	bool bIsSummoning = false;

	float DamagedStageThreshold = 0.9f;
	float InjuredStageThreshold = 0.4f;
	float NearDeathStageThreshold = 0.08f;

	//Used for keeping track of if the boss has passed a threshold during their damage state to spawn an enemy
	float CurrentEnemySpawnTarget;
	float EnemySpawnInterval;
	//Keeps track of how many enemies are still carrying the King
	int32 EnemiesLeft;

	FBossStages CurrentStage = FBossStages::InitialStage;

	TWeakObjectPtr<ADDBossEnemyBase> BossRef;
	TWeakObjectPtr<ADDEnemySpawner> SpawnerRef;
	TWeakObjectPtr<ADDGameModeBase> GameModeRef;
};
