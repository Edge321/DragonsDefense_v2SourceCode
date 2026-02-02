// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonsDefense_v2/Game/DDResettableInterface.h"
#include "DDEnemySpawner.generated.h"

class ADDSpawnRangeLimiterManager;
class UBillboardComponent;
class AEnemy;
class ADDHorizontalEnemy;
class ADDHorizontalSpawnHelper;
class ADDDisrupterEnemy;
class ADDBossEnemyBase;

enum class EDDStressMeterTypes : uint8;

DECLARE_DELEGATE_TwoParams(FOnSpawnEnemyForID, int32, bool);
DECLARE_DELEGATE_OneParam(FOnSpawnEnemy, EDDStressMeterTypes);
DECLARE_DELEGATE_OneParam(FOnDespawnEnemy, EDDStressMeterTypes);
DECLARE_DELEGATE_OneParam(FOnEnemyAttackingCastle, EDDStressMeterTypes);
DECLARE_DELEGATE_OneParam(FOnStrategicEnemySpawn, AEnemy*);
//Pass in boss pointer to initialize boss health bar
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossSpawn, ADDBossEnemyBase*, BossEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateBossHealth, float, TotalHealth, float, Health);

USTRUCT(BlueprintType)
struct FDDWaveInfo {

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Waves")
	TArray<TSubclassOf<AEnemy>> EnemiesToSpawn;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Waves")
	int32 WaveTrigger = 1;
	//Total enemies to spawn in the wave
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Waves")
	int32 NumberOfEnemies = 10;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Waves")
	float LowSpawnInterval = 3.0f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Waves")
	float HighSpawnInterval = 5.0f;
	//Sets which wave to be called first. Higher number = First wave to be called
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Waves")
	int32 Priority = 1;

};

UENUM(BlueprintType)
enum class EDDEnemyIDs : uint8 {
	NORMAL_ENEMY_ONE,
	NORMAL_ENEMY_TWO,
	NORMAL_ENEMY_THREE,
	DARK_KNIGHT,
	SHINOBI,
	FAMILIAR,
	ILLUSIONIST,
	TROJAN,
	TRAPPER,
	GOLDEN_ENEMY,
	MAGE_SNIPER,
	TINKERER,
	CRATER_CREATOR,
	DISRUPTER,
	MINI_BOSS,
	BOSS,
	ENEMY_TOTAL,
	EXTRA_SLOT_ENEMY,
	EXTRA_SLOT_ENEMY_TWO,
	NULL_ENEMY
};

/**
 * Does what the class's name says as well as
 * has a pool of enemies to manage
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDEnemySpawner : public AActor, public IDDResettableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* SpawnerIcon;

	
	//Area of where an enemy can be spawned on the Y-axis
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float SpawnAreaY = 1500.0f;
	//X-axis spawn range for where horizontal enemies can be spawned using the Horizontal Helper object location
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float HorizontalSpawnAreaX = 500.0f;
	//Used for reducing the spawn interval for fast enemy spawning
	//Not currently used, but maybe will be later on with waves implemented
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float SpawnTimeReducer;
	//Low end of the randomizer for push force on the x-axis of spawning in enemies (for Trojan enemy typically)
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float LowRandomXPushForce = 300.0f;
	//High end of the randomizer for push force on the x-axis of spawning in enemies (for Trojan enemy typically)
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float HighRandomXPushForce = 500.0f;
	//Low end of the randomizer for push force on the y-axis of spawning in enemies (for Trojan enemy typically)
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float LowRandomYPushForce = 1000.0f;
	//High end of the randomizer for push force on the y-axis of spawning in enemies (for Trojan enemy typically)
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	float HighRandomYPushForce = 3000.0f;
	//The key is the ID of the enemy class. E.g. 3 is Dark Knight
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TMap<EDDEnemyIDs, TSubclassOf<AEnemy>> EnemyEncyclopedia;
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	int32 EasyStarterWaveThreshold = 12;
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	int32 NormalStarterWaveThreshold = 8;
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	int32 HardStarterWaveThreshold = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	TArray<FDDWaveInfo> Waves;


public:

	virtual void ResetMods() override;
	//Pretty much a special function that spawns at a location 
	//and causes enemies to burst from the passed in location
	void SpawnEnemies(const TArray<TSubclassOf<AEnemy>> Enemies, const int32 EnemyAmount, FVector SpawnLocation);
	//Use the waves that have been defined in the blueprint. Used by The King if we aren't above the starter waves yet
	void UsePredefinedWaves();
	void PreSpawnSpecialEventEnemies();
	void PreSpawnBossEnemies();

	//Functions below are for The King and Downgrades pretty much
	void StartSpawn();
	void StopSpawn();

	void SetEnemiesToSpawn(const TArray<TSubclassOf<AEnemy>> NewEnemiesToSpawn);
	void SetMaxEnemySpawn(const int32 NewMaxEnemySpawn);
	void SetSpawnIntervals(const float NewLowSpawnInterval, const float NewHighSpawnInterval);
	void SetLowSpawnInterval(const float NewLowSpawnInterval);
	void SetHighSpawnInterval(const float NewHighSpawnInterval);
	void SetHealthMod(const float NewHealthMod);
	void SetShootCooldownMod(const float NewShootCooldownMod);
	void SetSoulValueMod(const float NewSoulValueMod);
	void SetMovementMod(const float NewMovementMod);
	void SetDamageMod(const float NewDamageMod);
	void SetMaxEnemySpawnMod(const float NewMaxEnemySpawnMod);
	void SetLowSpawnIntervalMod(const float NewLowSpawnIntervalMod);
	void SetHighSpawnIntervalMod(const float NewHighSpawnIntervalMod);
	void SetCurrentWaveInfo(const FDDWaveInfo Wave);
	//To be used by DDGameModeBase only!!!
	void SetCurrentWave(const int32 Wave);
	//Set off the flag to have an enemy strategically spawn
	//Use SetEnemySpawnRange after setting of the flag
	void SetStrategicSpawnFlag();
	/// <summary>
	/// For an upcoming strategic enemy spawn
	/// </summary>
	/// <param name="SpawnRange"></param>
	void SetEnemySpawnRange(FVector2D SpawnRange);
	
	TArray<TSubclassOf<AEnemy>> GetEnemiesToSpawn() const;
	int32 GetMaxEnemySpawn() const;
	int32 GetStarterWaveThreshold() const;
	float GetLowSpawnInterval() const;
	float GetHighSpawnInterval() const;
	TWeakObjectPtr<ADDDisrupterEnemy> GetDisrupterEnemy() const;
	TMap<EDDEnemyIDs, TSubclassOf<AEnemy>> GetEnemyEncyclopedia() const;

	//public for GameModeBase only
	void ToggleEnemyAttackBoxesEventFunction(const bool Visibility);

	UFUNCTION(BlueprintPure)
	const int32 GetCurrentWave() const;
	UFUNCTION(BlueprintPure)
	const int32 GetEnemiesLeftToSpawn() const;

	FOnSpawnEnemyForID OnSpawnEnemyForID;
	FOnSpawnEnemy OnSpawnEnemy;
	FOnDespawnEnemy OnDespawnEnemy;
	FOnEnemyAttackingCastle OnEnemyAttackingCastle;
	//Event for notifying that a strategic enemy has been spawned
	//Strategic placement should be taken care of by the receipient
	FOnStrategicEnemySpawn OnStrategicEnemySpawn;

	UPROPERTY(BlueprintAssignable)
	FOnBossSpawn OnBossSpawn;
	UPROPERTY(BlueprintAssignable)
	FOnBossDeath OnBossDeath;
	UPROPERTY(BlueprintAssignable)
	FOnUpdateBossHealth OnUpdateBossHealth;

private:

	TSubclassOf<AEnemy> SelectEnemyByRarity();
	
	//Used to initialize enemies to fetch information from them while also
	//bypassing Villagerpedia detection
	void SpawnEnemyOutsidePool(TPair<EDDEnemyIDs, TSubclassOf<AEnemy>> EnemyPair) const;
	void SpawnEnemy();
	//Spawn specific enemy without triggering StartSpawn()
	AEnemy* SpawnEnemy(TSubclassOf<AEnemy> EnemyClass);
	void AddEnemyToPool(AEnemy* Enemy);
	void RemoveEnemyFromPool(AEnemy* Enemy);
	void CleanPool();
	void SortWaves();
	FDDWaveInfo SearchWave() const;
	void SetWaveStats(const FDDWaveInfo Wave);
	void CheckEnemiesSpawned();
	void CheckIfWaveOver() const;
	void ApplyMods(AEnemy* Enemy) const;
	ADDHorizontalSpawnHelper* FindHorizontalSpawnHelper() const;
	void EnemyAttacking() const;
	const bool FindFloor(FHitResult& Hit) const;

	void BossDeathEventFunction();
	void UpdateHealthBossEventFunction(const float TotalHealth, const float Health) const;

	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void GameStartEventFunction();
	UFUNCTION()
	void WaveStartEventFunction();
	UFUNCTION()
	void WaveOverEventFunction();
	UFUNCTION()
	void FinalCardChosenEventFunction();

	FDelegateHandle BossHealthHandler;

	ADDHorizontalSpawnHelper* Helper;

	TWeakObjectPtr<ADDDisrupterEnemy> DisrupterRef;
	TWeakObjectPtr<ADDBossEnemyBase> BossEnemyRef;
	TWeakObjectPtr<ADDSpawnRangeLimiterManager> SpawnRangeLimiterRef;

	TArray<TSubclassOf<AEnemy>> EnemiesToSpawn;
	float LowSpawnInterval = 1.0f;
	float HighSpawnInterval = 3.0f;

	float OriginalSpawnInterval;
	FTimerHandle SpawnTimerHandle;
	TArray<AEnemy*> EnemyPool;

	int32 MaxEnemySpawn = 1;
	int32 EnemyCounter = 0;
	int32 CurrentWave = 1;
	//What wave to switch to The King handling waves
	int32 StarterWaveThreshold = 0;

	float HealthMod = 1;
	float ShootCooldownMod = 1;
	float SoulValueMod = 1;
	float MovementMod = 1;
	float DamageMod = 1;
	float MaxEnemySpawnMod = 1;
	float LowSpawnIntervalMod = 1;
	float HighSpawnIntervalMod = 1;

	bool bEnemiesToSpawnModified = false;
	bool bStrategicEnemySpawn = false;
	bool bIsFinalWave = false;
	bool bIsEnemyAttackBoxesVisible = false;

	FVector2D SpawnRange = FVector2D(0, 0);
};
