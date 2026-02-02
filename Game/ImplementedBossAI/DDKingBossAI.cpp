// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDKingBossAI.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Game/Debuffs/DDPermanentSlowDebuff.h"

void UDDKingBossAI::DoBossAI(ADDBossEnemyBase* Boss)
{
	//Kinda useless to do a tick function for something that is event based
	if (!bHasInitialized) {
		InitializeAI(Boss);
		bHasInitialized = true;
	}
}

void UDDKingBossAI::DoBossDeath(ADDBossEnemyBase* Boss)
{
	Boss->GetWorld()->GetTimerManager().ClearTimer(EnemySummonTimer);
}

void UDDKingBossAI::InitializeAI(ADDBossEnemyBase* Boss)
{
	EnemiesLeft = Boss->GetEnemySpawnAmount();
	EnemySpawnInterval = (DamagedStageThreshold - InjuredStageThreshold) / EnemiesLeft;
	CurrentEnemySpawnTarget = DamagedStageThreshold - EnemySpawnInterval;
	Boss->OnUpdateHealthBoss.AddUObject(this, &UDDKingBossAI::UpdateHealthBossEventFunction);
	BossRef = MakeWeakObjectPtr(Boss);
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(Boss->GetWorld()->GetAuthGameMode());
	if (GameMode) {
		SpawnerRef = MakeWeakObjectPtr(&(GameMode->GetEnemySpawner()));
		GameModeRef = MakeWeakObjectPtr(GameMode);
	}
}

void UDDKingBossAI::UpdateHealthBossEventFunction(const float TotalHealth, const float CurrentHealth)
{
	float HealthRatio = CurrentHealth / TotalHealth;

	//A very simple state machine, transitions on a straight line basically
	if (HealthRatio <= NearDeathStageThreshold) {
		CurrentStage = FBossStages::NearDeathStage;
	}
	else if (HealthRatio <= InjuredStageThreshold) {
		CurrentStage = FBossStages::InjuredStage;
	}
	else if (HealthRatio <= DamagedStageThreshold) {
		CurrentStage = FBossStages::DamagedStage;
	}

	ExecuteBossStage(TotalHealth, CurrentHealth);
}

void UDDKingBossAI::ExecuteBossStage(const float TotalHealth, const float CurrentHealth)
{
	if (!BossRef.IsValid()) return;

	float HealthRatio = CurrentHealth / TotalHealth;
	TArray<TSubclassOf<AEnemy>> EnemiesToSpawn;
	EnemiesToSpawn.Add(BossRef->GetEnemyClassSpawn());

	switch (CurrentStage) {
	case FBossStages::InitialStage:
		//King already does its own thing, nothing to see here
		break;
	case FBossStages::DamagedStage:
		if (HealthRatio <= CurrentEnemySpawnTarget) {
			if (EnemiesLeft <= 0) {
				UE_LOG(LogTemp, Error, TEXT("%s has ran out of enemies to spawn in DamagedStage, skipping..."), *(BossRef->GetName()))
				break;
			}
			BossRef->DisableMinionComponent();
			SpawnerRef->SpawnEnemies(EnemiesToSpawn, 1, BossRef->GetActorLocation());
			EnemiesLeft--;
			CurrentEnemySpawnTarget -= EnemySpawnInterval;
			AddPermanentSlowDebuff(BossRef->GetDamagedStateDebuffAmount());
		}
		break;
	case FBossStages::InjuredStage:
		//Make sure all enemies have been spawned before transitioning to this stage
		if (EnemiesLeft > 0) {
			BossRef->DisableAllMinionComponents();
			SpawnerRef->SpawnEnemies(EnemiesToSpawn, EnemiesLeft, BossRef->GetActorLocation());
			EnemiesLeft = 0;
		}

		if (!bIsSummoning) {
			BossRef->GetWorld()->GetTimerManager().SetTimer(EnemySummonTimer, this, &UDDKingBossAI::SummonEnemy, 2.0f, true);
			bIsSummoning = true;
			BossRef->SetMovementSpeed(BossRef->GetInjuredStateMovement());
		}
		break;
	case FBossStages::NearDeathStage:
		if (bIsSummoning) {
			BossRef->SetMovementSpeed(BossRef->GetNearDeathStateMovement());
			BossRef->GetWorld()->GetTimerManager().ClearTimer(EnemySummonTimer);
			bIsSummoning = false;
		}
		break;
	}
}

void UDDKingBossAI::SummonEnemy()
{
	if (!BossRef.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("No existing BossRef for %s, ignoring function..."), *GetName())
		return;
	}

	if (BossRef->GetSummonSoulAmount() > GameModeRef->GetSouls()) {
		UE_LOG(LogTemp, Log, TEXT("%s cannot summon due to lack of souls!"), *GetName())
		return;
	}

	GameModeRef->UpdateSouls(-BossRef->GetSummonSoulAmount());

	TArray<TSubclassOf<AEnemy>> EnemiesToSpawn;
	EnemiesToSpawn.Add(BossRef->GetEnemyClassSpawn());
	SpawnerRef->SpawnEnemies(EnemiesToSpawn, 1, BossRef->GetActorLocation());
}

void UDDKingBossAI::AddPermanentSlowDebuff(const float DebuffAmount)
{
	UDDTimeBasedDebuffBase* Debuff = NewObject<UDDTimeBasedDebuffBase>(this, UDDPermanentSlowDebuff::StaticClass());
	if (Debuff) {
		Debuff->SetDebuffAmount(DebuffAmount);
		Debuff->SetDebuffLength(99999999.0f);
		BossRef->AddDebuff(Debuff);
	}
}
