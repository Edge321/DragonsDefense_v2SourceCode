// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DDBossEnemyBase.generated.h"

DECLARE_DELEGATE(FOnDeathBoss);
//TotalHealth, Health
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdateHealthBoss, float, float);

class UBillboardComponent;
class UBossMageMinionComponent;
class UDDBossAIBase;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDBossEnemyBase : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADDBossEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* BossIconBillboard;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss")
	TSubclassOf<UDDBossAIBase> BossAIClass;
	//List of randomized skins the boss enemy can have on spawn
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss")
	TArray<UStaticMesh*> BossSkins;
	//How much does the boss's health scale in relation to the wave number?
	//Close to 1 is a linear increase (Essentially Base Health + CurrentWave)
	//Close to 2 is a quadratic increase
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|Health", meta = (ClampMin = "1.0", ClampMax = "2.0", UIMin = "1.0", UIMax = "2.0"))
	float HealthExponentScaling = 1.5f;
	//Class of enemy that spawns when boss gets hurt or dies
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|AI")
	TSubclassOf<AEnemy> EnemyClassSpawn;
	//How many enemies should spawn on death
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|AI")
	int32 EnemySpawnAmount = 6;
	//How much of a speed debuff occurs each time an enemy is spawned during damaged state
	//Close to 0 is a major speed debuff
	//Close to 1 is a minor speed debuff
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|AI", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DamagedStateDebuffAmount = 0.95f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|AI")
	float InjuredStateMovement = 80.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|AI")
	float NearDeathStateMovement = 40.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|AI")
	int32 SummonSoulAmount = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|AI")
	TArray<int32> MinionKillOrder;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|Projectile")
	FVector BossProjectileMovement;
	//Boss projectile is meant to insta-kill any placeables
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Boss|Projectile")
	float BossProjectileDamage = -999999.0f;

	virtual void Shoot() override;
	virtual void ShootPlaceable() override;
	virtual void OnDeath() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void UpdateHealth(const float HealthModifier) override;

	//leave at -1 for a random removal
	void DisableMinionComponent(const int32 Index = -1);
	void DisableAllMinionComponents();

	const TSubclassOf<AEnemy> GetEnemyClassSpawn() const;
	const int32 GetEnemySpawnAmount() const;
	const float GetTotalHealth() const;
	const float GetDamagedStateDebuffAmount() const;
	const float GetInjuredStateMovement() const;
	const float GetNearDeathStateMovement() const;
	const int32 GetSummonSoulAmount() const;

	FOnDeathBoss OnDeathBoss;
	FOnUpdateHealthBoss OnUpdateHealthBoss;

private:

	void ShootProjectile() const;

	float TotalHealth;

	UPROPERTY()
	UDDBossAIBase* BossAI;

	TArray<UBossMageMinionComponent*> Minions;
};
