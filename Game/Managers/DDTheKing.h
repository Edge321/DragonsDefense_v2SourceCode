// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Characters/DDSecretStats.h"

#include "DDTheKing.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedWaveCalculation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialEnemyComing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStressChange, float, CurrentStressLevel, float, MaxStressLevel);

class AEnemy;
class ADDPlayer;
class ADDGameModeBase;
class ADDPlaceableManager;
class ADDNegativeSoulShopManager;
class ADDOccupationSquare;
class ADDSpawnRangeLimiterManager;
class UDDSoulShopWidget;
class UBillboardComponent;
class UDDDowngrade;

enum class EDDEnemyType : uint8;
enum class EDDStressMeterTypes : uint8;

UENUM(BlueprintType)
enum class EDDPlayerStyle : uint8 {
	Aggressive,
	Defensive,
	Supportive,
	Generalist
};

USTRUCT(BlueprintType)
struct FDDPlayerSecretStats {

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float CalculatedOffense;
	UPROPERTY(BlueprintReadOnly)
	float CalculatedDefense;
	UPROPERTY(BlueprintReadOnly)
	float CalculatedSupport;
	UPROPERTY(BlueprintReadOnly)
	EDDPlayerStyle Style;
};

//Euler's number pog!!!
constexpr float e = 2.71828;

UCLASS()
class DRAGONSDEFENSE_V2_API ADDTheKing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDTheKing();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* KingIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Default Wave Values")
	float DefaultHighSpawnInterval = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Default Wave Values")
	float DefaultLowSpawnInterval = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Default Wave Values")
	int32 DefaultNumberOfEnemies = 15;
	//Array that compromises of enemies that should not be added to the pool of enemies using Enemy ID
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow")
	TArray<EDDEnemyIDs> EnemiesNotToAdd;
	//Array that compromises of enemies that will always be added to the pool of enemies using Enemy ID
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow")
	TArray<EDDEnemyIDs> EnemiesToAlwaysAdd;
	//Chance of being put in enemy pool out of 100%
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Uncontrollables", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float GoldenEnemyChance = 5.0f;
	//How much to increase the golden enemy chance if it was not chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Uncontrollables", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float GoldenEnemyChanceIncrease = 0.5f;
	//Every n waves is when the disrupter appears
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Uncontrollables")
	int32 DisrupterAppearanceWave = 10;
	//The lower end of the randomizer for the first special wave
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Special Wave Check")
	int32 LowerStartingSpecialWaveRandomizer = 10;
	//The higher end of the randomizer for the first special wave
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Special Wave Check")
	int32 HigherStartingSpecialWaveRandomizer = 15;
	//The lower end of the randomizer when a new special wave is chosen
	//The number is added to the current special wave
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Special Wave Check")
	int32 LowerSpecialWaveRandomizer = 10;
	//The higher end of the randomizer for a new special wave is chosen
	//The number is added to the current special wave
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Special Wave Check")
	int32 HigherSpecialWaveRandomizer = 15;
	//How strong do beginning souls affect the total stability points?
	//Close to 0 is less stability points
	//Higher than 5 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stability Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float BeginningSoulsFactor = 0.5f;
	//How strong do total souls affect the total stability points?
	//Close to 0 is less stability points
	//Higher than 1 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stability Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float TotalSoulsFactor = 0.5f;
	//How strong do beginning placeables placed affect the total stability points?
	//Close to 0 is less stability points
	//Higher than 5 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stability Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float BeginningPlaceablesPlacedFactor = 1.2f;
	//How strong do total placeables placed affect the total stability points?
	//Close to 0 is less stability points
	//Higher than 1 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stability Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float TotalPlaceablesFactor = 0.5f;
	//How strong do total player health affect the total stability points?
	//Close to 0 is less stability points
	//Higher than 5 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stability Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float TotalPlayerHealthFactor = 2.0f;
	//How strong does the health ratio affect total stability points? 
	//Close to 0 is less stability points
	//Higher than 1 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stability Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float HealthRatioFactor = 1.0f;
	//How strong does wave number affect current soul amount in the total stability points formula? 
	//Close to 0 is soul amount is least effective in the formula
	//Close to 5 is soul amount has great effect on the formula
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stability Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float WaveNumberFactor = 1.0f;
	//How strong do current placeable amount affect the total pity points?
	//Typically, you want to do this factor around the same as TotalPlaceablesPlacedFactor
	//Close to 0 is less stability points
	//Higher than 5 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float CurrentPlaceableAmountFactor = 1.2f;
	//How strong do current player health affect the total pity points?
	//Typically, you want to do this factor around the same as TotalPlayerHealthFactor
	//Close to 0 is less stability points
	//Higher than 5 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float CurrentPlayerHealthFactor = 2.0f;
	//How strong do current player health affect the total pity points?
	//Typically, you want to do this factor around the same as TotalSoulsFactor
	//Close to 0 is less stability points
	//Higher than 5 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float CurrentSoulsFactor = 0.5f;
	//How strong does downgrade being active affect total pity points? 
	//Close to 0 is less stability points
	//Close to 1.5 is more stability points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "1.5", UIMin = "0.00001", UIMax = "1.5"))
	float DowngradeFactor = 1.1f;
	//How strong does the soul ratio affect pity points? 
	//Close to 0 is less pity points
	//Close to 5 is more pity points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float SoulRatioFactor = 1.0f;
	//How strong does the placeable ratio affect pity points?
	//Close to 0 is less pity points
	//Close to 5 is more pity points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "5.0", UIMin = "0.00001", UIMax = "5.0"))
	float PlaceableRatioFactor = 1.0f;
	//How strong does easy difficulty affect pity points?
	//Close to 1 is no affect on pity points
	//Close to 3 is more pity points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "1.0", ClampMax = "3.0", UIMin = "1.0", UIMax = "3.0"))
	float EasyFactor = 1.5f;
	//How strong does hard difficulty affect pity points?
	//Close to 0 is no affect on pity points
	//Close to 1 is normal amount of pity points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HardFactor = 0.5f;
	//How strong does reward being active affect pity points?
	//Close to 0 is less pity points
	//Close to 1 is no affect on pity points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.00001", ClampMax = "1.0", UIMin = "0.00001", UIMax = "1.0"))
	float RewardFactor = 0.75f;
	//How strong are pity points limited the higher the wave?
	//Close to 0 is pity points are never limited by wave number
	//Close to 3 is pity points are very limited by wave number
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "0.0", ClampMax = "3.0", UIMin = "0.0", UIMax = "3.0"))
	float PityPointExponentLimiter = 1.7f;
	//How strong does low player health affect pity points?
	//Close to 1 is normal affect on pity points
	//Close to 3 is great affect on pity points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "1.0", ClampMax = "3.0", UIMin = "1.0", UIMax = "3.0"))
	float LowPlayerHealthFactor = 2.0f;
	//High limit of how much the ratio of souls, placeables, or health can affect pity points
	//Higher means great affect on pity points
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Formula Strength", meta = (ClampMin = "1.0", ClampMax = "3.0", UIMin = "1.0", UIMax = "3.0"))
	float HighPityEffectLimit = 2.0f;
	//Max size of enemy pool that the king can normally do
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Pool Size")
	int32 MaxEnemyPoolSize = 6;
	//How much does wave number affect enemy pool size
	//Close to 0 is no effect
	//Close to 1 is all the effect
	//Note: SafetyPoints weight is the inverse of this weight. i.e. SafetyPoints + WaveNumberWeight = 1
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Pool Size", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float WaveNumberWeight = 0.6f;
	//Initial max wave number for enemy pool size
	//Close to 0 is enemy pool size will reach MaxEnemyPoolSize sooner
	//The higher, the longer it takes for enemy pool size to increase
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Pool Size")
	int32 InitialMaxWaveNumber = 90;
	//How fast does InitialMaxWaveNumber get closer to current wave number
	//Close to 0 takes longer to reach same number as current wave number
	//Close to 0.2 takes a shorter amount of time to reach as number as current wave number
	//Note: The closer the current wave number is to the max number calculated, the sooner max enemy pool size will be reached
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Pool Size", meta = (ClampMin = "0.00001", ClampMax = "0.2", UIMin = "0.00001", UIMax = "0.2"))
	float ExponentialDecayExponent = 0.015f;
	//How slow does easy difficulty affect getting to max enemy pool size
	//Close to 0 is no affect
	//Higher number is greatly affects how slow it takes
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Pool Size")
	int32 EasyWaveNumberModifier = 15;
	//How fast does hard difficulty affect getting to max enemy pool size
	//Close to 0 is no affect
	//Higher number is greatly affects how fast it takes
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Pool Size")
	int32 HardWaveNumberModifier = 10;
	//What wave threshold until The King puts in more than SpecialEnemyLimitNumber special enemy type in the enemy type pool for easy difficulty
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool")
	int32 SpecialEnemyLimitEasyWaveNumberThreshold = 12;
	//What wave threshold until The King puts in more than SpecialEnemyLimitNumber special enemy type in the enemy type pool for normal difficulty
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool")
	int32 SpecialEnemyLimitNormalWaveNumberThreshold = 10;
	//What wave threshold until The King puts in more than SpecialEnemyLimitNumber special enemy type in the enemy type pool for hard difficulty
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool")
	int32 SpecialEnemyLimitHardWaveNumberThreshold = 8;
	//What is the limit of special enemies until the special enemy limit wave threshold has passed
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool")
	int32 SpecialEnemyLimit = 1;
	//How much of the base weight do basic enemies have in being picked for the enemy pool
	//Higher weight means higher chance of being chosen for enemy pool
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool", meta = (ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0"))
	float BaseBasicEnemyWeight = 7.0f;
	//How much of the base weight do special enemies have in being picked for the enemy pool
	//Higher weight means higher chance of being chosen for enemy pool
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool", meta = (ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0"))
	float BaseSpecialEnemyWeight = 2.0f;
	//How much effect does easy difficulty have on basic enemies being picked for the enemy pool
	//Close to 1 is no affect on basic enemies being chosen
	//Higher number means greater chance of basic enemies being chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool", meta = (ClampMin = "1.0", ClampMax = "5.0", UIMin = "1.0", UIMax = "5.0"))
	float EasyBasicEnemyWeightModifier = 1.2f;
	//How much effect does hard difficulty have on basic enemies being picked for the enemy pool
	//Close to 0 means greater chance of basic enemies not being chosen
	//Higher number means greater chance of basic enemies being chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float HardBasicEnemyWeightModifier = 0.9f;
	//What should be the base amount when determining how much pity points affect basic enemy weight
	//Close to 0.1 means it takes a lot of pity points to increase the chances of basic enemies being chosen
	//Close to 0.9 means it takes not much pity points to increase the chances of basic enemies being chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool", meta = (ClampMin = "0.1", ClampMax = "0.9", UIMin = "0.1", UIMax = "0.9"))
	float BaseBasicEnemyPityPoints = 0.75f;
	//What should be the base amount when determining how much the wave number affects special enemy weight
	//Close to 0.1 means it takes a high wave number to increase the chances of special enemies being chosen
	//Close to 0.9 means it takes a low wave number to increase the chances of special enemies being chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool", meta = (ClampMin = "0.1", ClampMax = "0.9", UIMin = "0.1", UIMax = "0.9"))
	float BaseSpecialEnemyWaveNumber = 0.75f;
	//How much should basic enemy chance be decreased after a basic enemy has been picked?
	//Decreases based on multiplication
	//Close to 0 means severely decrease chance of basic enemy being chosen again
	//Close to 1 means barely any decrease chance for basic enemy being chosen again
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float BasicEnemyDecreaserModifier = 0.85f;
	//What wave threshold until The King starts considering attacking special enemies in the enemy pool
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies")
	int32 AttackingSpecialEnemyWave = 9;
	//The weight that determines an enemy that completely counters the player's play style 
	//is sorted on the high end of an array of countering. Used by multiplication
	//Close to 0 is typically being sorted on the low end of an array
	//Close to 2 is typically being sorted on the high end of an array
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float CompleteCounterWeight = 1.5f;
	//The weight that determines an enemy that is equal to the player's play style 
	//is sorted in the middle an array of countering. Used by multiplication
	//Close to 0 is typically being sorted on the low end of an array
	//Close to 2 is typically being sorted on the high end of an array
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float EqualCounterWeight = 1.0f;
	//The weight that determines an enemy that does not at all counter the player's play style 
	//is sorted on the low end of an array of countering. Used by multiplication
	//Close to 0 is typically being sorted on the low end of an array
	//Close to 2 is typically being sorted on the high end of an array
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float NoCounterWeight = 0.5f;
	//Determines how in favor bigger counter enemies are to be chosen for the enemy pool
	//Close to 1 is no enemies are favored (complete randomness)
	//Close to 2 is complete counter enemies are favored
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "1.0", ClampMax = "2.0", UIMin = "1.0", UIMax = "2.0"))
	float IndexBiasStrength = 1.2f;
	//Threshold that determines if the player did too well in the current wave,
	//resulting in better countering the player's style
	//Determined by taking (PityPoints/StabilityPoints)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PlayerSuccessThreshold = 0.25f;
	//How much is the index bias going to be increased based on
	//if the player is doing too well in a wave
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PlayerSuccessIncrease = 0.1f;
	//Threshold that determines if the player did poorly in the current wave
	//Determined by taking (PityPoints/StabilityPoints)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PlayerFailureThreshold = 0.75f;
	//How much is the index bias going to be decreased based on
	//if the player is doing poorly in a wave
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float PlayerFailureDecrease = 0.15f;
	//How much does easy difficulty tone down bias decrease calculation if a 
	//hard counter enemy is chosen, such that next time a no counter enemy is chosen?
	//Close to 0 is no influence
	//Close to 1 is very high influence
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float EasyEnemyCounterBiasAdjuster = 0.1f;
	//How much does hard difficulty tone up bias increase calculation if a 
	//no counter enemy is chosen, such that next time a hard counter enemy is chosen?
	//Close to 0 is no influence
	//Close to 1 is very high influence
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Type Pool | Specific Enemies", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HardEnemyCounterBiasAdjuster = 0.1f;
	//How much does easy difficulty affect the calculation of enemy amount?
	//Close to 0 means severely decrease enemy amount calculation
	//Close to 1 means barely decrease enemy amount calculation
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Amount", meta = (ClampMin = "0.0001", ClampMax = "1.0", UIMin = "0.0001", UIMax = "1.0"))
	float EasyWaveEnemyAmountModifier = 0.8f;
	//How much does hard difficulty affect the calculation of enemy amount?
	//Close to 1 means barely increase enemy amount calculation
	//Close to 2 means severly increase enemy amount calculation
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Amount", meta = (ClampMin = "1.0", ClampMax = "2.0", UIMin = "1.0", UIMax = "2.0"))
	float HardWaveEnemyAmountModifier = 1.2f;
	//How much does the current wave number factor affect the calculation of enemy amount?
	//Close to 1 means barely affects enemy amount calculation
	//Close to 2 means greatly affects enemy amount calculation
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Amount", meta = (ClampMin = "1.0", ClampMax = "2.0", UIMin = "1.0", UIMax = "2.0"))
	float EnemyAmountWaveFactor = 1.1f;
	//How much is the quadratic growth slowed down by?
	//Close to 1 means normal quadratic growth formula
	//Close to 100 means significantly slower quadratic growth formula
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Amount", meta = (ClampMin = "1.0", ClampMax = "100.0", UIMin = "1.0", UIMax = "100.0"))
	float EnemyAmountDivider = 5.0f;
	//What is the base amount of enemies that will be added to the enemy amount calculation?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Enemy Amount")
	float BaseEnemyAmount = 6.0f;
	//What is the starting wild card appearance wave?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wild Card")
	int32 WildCardAppearanceWave = 5;
	//What is the lowest end for adding on to the wild card appearance wave?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wild Card")
	int32 LowWildCardAddition = 3;
	//What is the highest end for adding on to the wild card appearance wave?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wild Card")
	int32 HighWildCardAddition = 7;
	//How much does the enemy reduction pity reward multiplier increase at each increment of a wave
	//Close to 0 is pity rewards barely increase as waves progress
	//Close to 1 is pity rewards increase dramatically as waves progress
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float EnemyReductionPityRewardMultiplierIncrement = 0.2f;
	//How much does the enemy pool removal pity reward multiplier increase at each increment of a wave
	//Close to 0 is pity rewards barely increase as waves progress
	//Close to 1 is pity rewards increase dramatically as waves progress
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float EnemyPoolPityRewardMultiplierIncrement = 0.2f;
	//How much does the stress meter minimum increase pity reward multiplier increase at each increment of a wave
	//Close to 0 is pity rewards barely increase as waves progress
	//Close to 1 is pity rewards increase dramatically as waves progress
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float StressMeterPityRewardMultiplierIncrement = 0.1f;
	//Percentage of reducing the total amount of enemies that will be spawned
	//Close to 0 is no reduction
	//Close to 1 is 100% reduction
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float BaseEnemyAmountReduction = 0.05;
	//What is the low end of the randomization of increasing the minimum stress meter threshold?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float MinimumStressMeterIncrease = 0.5f;
	//What is the low end of the randomization of increasing the maximum stress meter threshold?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float MaximumStressMeterIncrease = 1.0f;
	//How much of a chance does base enemy amount reduction have in being chosen first in line of pity deduction?
	//Close to 100 is high chance of being chosen
	//Close to 0 is low chance of being chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float BaseEnemyAmountReductionWeight = 50.0f;
	//How much of a chance does enemy pool removal have in being chosen first in line of pity deduction?
	//Close to 100 is high chance of being chosen
	//Close to 0 is low chance of being chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float EnemyPoolRemovalWeight = 1.0f;
	//How much of a chance does stress meter minimum increase have in being chosen first in line of pity deduction?
	//This will result in The King being less aggressive during a wave if the player is doing too good
	//Close to 100 is high chance of being chosen
	//Close to 0 is low chance of being chosen
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float StressMeterMinimumIncreaseWeight = 10.0f;
	//How many pity points does base enemy amount reduction cost?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction | Cost")
	int32 BaseEnemyAmountReductionCost = 10;
	//How many pity points does enemy pool removal cost?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction | Cost")
	int32 EnemyPoolRemovalCost = 150;
	//How many pity points does stress meter minimum increase cost?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Pity Points Reduction | Cost")
	int32 StressMeterMinimumIncreaseCost = 300;
	//Percentage of enemies that are assumed to be basic for the calculated total enemy amount on easy difficulty
	//Close to 0 is all enemies are assumed to be special
	//Close to 1 is all enemies are assumed to be basic
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wave Jump", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float EasyBasicEnemyRatio = 0.85f;
	//Percentage of enemies that are assumed to be basic for the calculated total enemy amount on normal difficulty
	//Close to 0 is all enemies are assumed to be special
	//Close to 1 is all enemies are assumed to be basic
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wave Jump", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float NormalBasicEnemyRatio = 0.8f;
	//Percentage of enemies that are assumed to be basic for the calculated total enemy amount on hard difficulty
	//Close to 0 is all enemies are assumed to be special
	//Close to 1 is all enemies are assumed to be basic
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wave Jump", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HardBasicEnemyRatio = 0.75f;
	//Bonus souls for enemies, a multiplier for the total enemies calculated
	//E.g. CalculatedEnemyAmount * SpecialEnemySoulBonus
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wave Jump")
	int32 SpecialEnemySoulBonus = 2;
	//What is the interval of the set wave jumps?
	//As of this writing, they are 1, 11, 21, etc., so 10 wave intervals
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Wave Jump")
	int32 WaveJumpInterval = 10;

	//Represents how far the player is in-game and how well they are doing
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	int32 StabilityPoints;
	//Represents how well the player is doing so far
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	int32 SafetyPoints;
	//Represents how poor the player is doing so far
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	int32 PityPoints;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	int32 SpecialWaveNumber = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	int32 CurrentWildCardWave = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	int32 EnemyPoolSize;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	bool IsSpecialWave = false;
	//What is the player's current style?
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	FDDPlayerSecretStats SecretStats;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	FDDWaveInfo CalculatedWaveInfo;
	//Use CalculatePlayerUpgradeStats first (only once) before using this variable!
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Active Stats")
	FDDSecretStats PlayerCalculatedStats = {
		0, 0, 0
	};

	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats")
	float ActualMinStressMeterThreshold = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats")
	float ActualMaxStressMeterThreshold = 100.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats")
	float CurrentStressLevel = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats")
	float CastleStressBucketLevel = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float EnemySpawnStressRate = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float EnemyDeathStressRate = -2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float CastleAttackedStressRate = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float PlaceableDeathStressRate = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float PlaceableSpawnStressRate = -0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Stats", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float PlayerUpgradeStressRate = -0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares")
	TSubclassOf<ADDOccupationSquare> OccupationSquareClass;
	//Defines area where occupation squares are limited to on an X-axis range. (Min X, Max X)
	//Different than Y-axis variable cus battlefield not exactly symmetrical like Y-axis
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares")
	FVector2D OccupationSquareLimitXAxis;
	//Defines area where occupation squares are limited to on the Y-axis
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares")
	float OccupationSquareLimitYAxis;
	//How many occupation squares to be included in the area? (X by Y squares)
	//X is the amount of squares per column
	//In other words, the X and Y are reversed compared to typical layout (Fucked up on the occupation square creation)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares")
	FVector2D OccupationSquareAmount;
	//Used to scale threat level of a placeable based on how close their occupation square is to the castle
	//Starting from 1, and then adding X amount for each occupation square that is closer to the castle
	//E.g. An occupation square that is at the top of the row (near enemy spawn),
	//threat level is then ((1 + 0) * ThreatLevel)
	//An occupation square at the bottom row (near the castle),
	//threat level is then ((1 + X) * ThreatLevel)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float ThreatLevelMultiplierScaling = 0.1f;
	//Used to scale offense secret stat on placeables depending on how close the occupation square is to the castle
	//The amount increases (or decreases) based on each square closer to the castle
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float SecretOffenseScaling = 5.0f;
	//The base amount to start from for scaling offense for each square closer to the castle
	//Starting from the square furthest from the castle (0th row of squares)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float BaseOffense = 20.0f;
	//Used to scale defense secret stat on placeables depending on how close the occupation square is to the castle
	//The amount increases (or decreases) based on each square closer to the castle
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float SecretDefenseScaling = 5.0f;
	//The base amount to start from for scaling offense for each square closer to the castle
	//Starting from the square furthest from the castle (0th row of squares)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float BaseDefense = -15.0f;
	//Used to scale defense secret stat on placeables depending on how close the occupation square is to the castle
	//The amount increases (or decreases) based on each square closer to the castle
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float SecretSupportScaling = 2.0f;
	//The base amount to start from for scaling offense for each square closer to the castle
	//Starting from the square furthest from the castle (0th row of squares)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Occupation Squares", meta = (ClampMin = "-100.0", ClampMax = "100.0", UIMin = "-100.0", UIMax = "100.0"))
	float BaseSupport = 0.0f;
	//Multiplier for all the stress rates on easy mode
	//Close to 0 is stress rates are almost non-existent (Aggressive spawning)
	//Close to 2 is stress rates are significant (Pacifist spawning)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float EasyStressRateMultiplier = 1.1f;
	//Multiplier for all the stress rates on hard mode
	//Close to 0 is stress rates are almost non-existent (Aggressive spawning)
	//Close to 2 is stress rates are significant (Pacifist spawning)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float HardStressRateMultiplier = 0.8f;
	//How much does each event that increases the stress meter have an affect?
	//Close to 0 means barely increases the stress meter (meaning the player will always be stressed out)
	//Higher than 1 means greatly increases the stress meter (meaning the player will always be chillin)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter")
	float StressMeterIncreaserFactor = 1.0f;
	//Where does the base of the lower end of the spawn rate begin?
	//Start point is calculated by: (e^LowerSpawnIntervalBaseExponent)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Spawn Interval Base Function | Low Spawn Interval", meta = (ClampMin = "0.1", ClampMax = "2.0", UIMin = "0.1", UIMax = "2.0"))
	float LowSpawnIntervalBaseExponent = 1.4f;
	//How fast does the lower spawn interval base reach 0 the higher the wave number?
	//Close to 0 means a very slow descent to reach 0 when reaching higher wave numbers
	//Close to 1 means a very fast descent to reach 0 when reaching higher wave numbers
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Spawn Interval Base Function | Low Spawn Interval", meta = (ClampMin = "0.0001", ClampMax = "1.0", UIMin = "0.0001", UIMax = "1.0"))
	float LowSpawnIntervalBaseDecreaseRate = 0.1f;
	//What is the offset of the starting lower spawn interval base on the x-axis? 
	//Based on LowerSpawnIntervalBaseExponent
	//E.g. LowerSpawnIntervalBaseXOffset = 1
	//The y-intercept of the function is moved 1 unit to the right on the x-axis now
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Spawn Interval Base Function | Low Spawn Interval")
	int32 LowSpawnIntervalBaseXOffset = 1;
	//Where does the high spawn interval base begin?
	//Start point is calculated by: (e^HighSpawnIntervalBaseExponent)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Spawn Interval Base Function | High Spawn Interval", meta = (ClampMin = "0.1", ClampMax = "2.0", UIMin = "0.1", UIMax = "2.0"))
	float HighSpawnIntervalBaseExponent = 1.6f;
	//How fast does the high spawn interval base reach 0 the higher the wave number?
	//Close to 0 means a very slow descent to reach 0 when reaching higher wave numbers
	//Close to 1 means a very fast descent to reach 0 when reaching higher wave numbers
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Spawn Interval Base Function | High Spawn Interval", meta = (ClampMin = "0.0001", ClampMax = "1.0", UIMin = "0.0001", UIMax = "1.0"))
	float HighSpawnIntervalBaseDecreaseRate = 0.03f;
	//What is the offset of the starting high spawn interval base on the x-axis? 
	//Based on HighSpawnIntervalBaseExponent
	//E.g. HighSpawnIntervalBaseXOffset = 1
	//The y-intercept of the function is moved 1 unit to the right on the x-axis now
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Spawn Interval Base Function | High Spawn Interval")
	int32 HighSpawnIntervalBaseXOffset = 1;
	//How much does the base low end spawn interval decrease/increase as the stress meter increases
	//Decrease/Increase inflection point is based on the combination of LowSpawnStressCorrectedBase and LowSpawnStressCorrectedExponent
	//Look at Desmos.com, plug in the equation for what to look for
	//Close to 0 makes the inflection point late in the stress meter, mostly decreasing the low spawn interval
	//Close to 0.5 makes the inflection point early in the stress meter, mostly increasing the low spawn interval
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Corrected Spawn Interval Function | Low Spawn Interval", meta = (ClampMin = "0.0", ClampMax = "0.5", UIMin = "0.0", UIMax = "0.5"))
	float LowSpawnStressCorrectedExponent = 0.015f;
	//What is the starting point of the stress correction function for low spawn interval
	//Close to 1 means a high starting point. In terms of the stress meter, a small decrease at low stress, big increase at high stress
	//Close to 5 means a low starting point. In terms of the stress meter, a big decrease at low stress, small increase at high stress
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Corrected Spawn Interval Function | Low Spawn Interval", meta = (ClampMin = "1.0", ClampMax = "5.0", UIMin = "1.0", UIMax = "5.0"))
	float LowSpawnStressCorrectedBase = 1.75f;
	//Limit for how low the low spawn interval can be after calculations
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Corrected Spawn Interval Function | Low Spawn Interval")
	float LowSpawnIntervalClamp = 0.5f;
	//How much does the base high end spawn interval decrease/increase as the stress meter increases
	//Decrease/Increase inflection point is based on the combination of HighSpawnStressCorrectedBase and HighSpawnStressCorrectedExponent
	//Look at Desmos.com, plug in the equation for what to look for
	//Close to 0 makes the inflection point late in the stress meter, mostly decreasing the low spawn interval
	//Close to 0.5 makes the inflection point early in the stress meter, mostly increasing the low spawn interval
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Corrected Spawn Interval Function | High Spawn Interval", meta = (ClampMin = "0.0", ClampMax = "0.5", UIMin = "0.0", UIMax = "0.5"))
	float HighSpawnStressCorrectedExponent = 0.015f;
	//What is the starting point of the stress correction function for high spawn interval
	//Close to 1 means a high starting point. In terms of the stress meter, a small decrease at low stress, big increase at high stress
	//Close to 5 means a low starting point. In terms of the stress meter, a big decrease at low stress, small increase at high stress
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Corrected Spawn Interval Function | High Spawn Interval", meta = (ClampMin = "1.0", ClampMax = "5.0", UIMin = "1.0", UIMax = "5.0"))
	float HighSpawnStressCorrectedBase = 1.75f;
	//Limit for how low the high spawn interval can be after calculations
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Corrected Spawn Interval Function | High Spawn Interval")
	float HighSpawnIntervalClamp = 0.75f;
	//How much does easy difficulty contribute to the chance of a strategic spawn
	//Keep it on the lower end to keep it fair
	//Close to 0 means no contribution
	//Close to 100 means high contribution (always a strategic spawn at this point)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float EasyStrategicSpawnChance = 5.0f;
	//How much does normal difficulty contribute to the chance of a strategic spawn
	//Keep it on the lower end to keep it fair
	//Close to 0 means no contribution
	//Close to 100 means high contribution (always a strategic spawn at this point)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float NormalStrategicSpawnChance = 10.0f;
	//How much does hard difficulty contribute to the chance of a strategic spawn
	//Keep it on the lower end to keep it fair
	//Close to 0 means no contribution
	//Close to 100 means high contribution (always a strategic spawn at this point)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float HardStrategicSpawnChance = 15.0f;
	//The max value for the stress meter to reduce strategic spawn. The higher the player's stress, 
	//the more strategic spawn will be reduced
	//Close to -100 is severly reduce strategic spawn (no strategic spawn to occur ever)
	//Close to 0 is no effect on strategic spawn
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "-100.0", ClampMax = "0.0", UIMin = "-100.0", UIMax = "0.0"))
	float StressMeterStrategicSpawnMax = -10.0f;
	//The max value for the placeable amount to reduce strategic spawn. The higher the placeable amount,
	//the more strategic spawn will be increased
	//Close to 0 is no effect on strategic spawn
	//Close to 100 is severely increase strategic spawn (always strategically spawning)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float PlaceableAmountStrategicSpawnMax = 10.0f;
	//The max value for the placeable ratio which will increase strategic spawn
	//A very low value is no effect on strategic spawn
	//A very high value is severly increase strategic spawn (Uncapped strategic spawn increase)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter")
	int32 PlaceableRatioOverflowMax = 5.0f;
	//The max value for the soul amount to reduce strategic spawn. The higher the soul amount,
	//the more strategic spawn will be increased
	//Close to 0 is no effect on strategic spawn
	//Close to 100 is severely increase strategic spawn (always strategically spawning)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float SoulAmountStrategicSpawnMax = 10.0f;
	//The max value for the soul ratio which will increase strategic spawn
	//A very low value is no effect on strategic spawn
	//A very high value is severly increase strategic spawn (Uncapped strategic spawn increase)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter")
	int32 SoulRatioOverflowMax = 5.0f;
	//How fast does castle damage causing stress drain per second?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Bucket")
	float CastleStressBucketDrainRate = 5.0f;
	//How fast does the castle damage stress bucket drain completely (in seconds)?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Bucket")
	float TotalCastleStressBucketDrainTime = 5.0f;
	//How much time (in seconds) needs to pass without any castle damage stress to start draining the stress bucket?
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Pool Flow | Stress Flow | Stress Meter | Stress Bucket")
	float CastleStressBucketDrainDelay = 3.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnFinishedWaveCalculation OnFinishedWaveCalculation;
	UPROPERTY(BlueprintAssignable)
	FOnSpecialEnemyComing OnSpecialEnemyComing;
	UPROPERTY(BlueprintAssignable)
	FOnStressChange OnStressChange;

	//For the placeable preview only!!!
	TArray<AActor*> GetOccupationSquares() const;

	void AddDowngradeFunction(std::function<void()> Func);

	//For Downgrade functions only!!!!
	FDDWaveInfo GetCalculatedWaveInfo() const;
	void SetCalculatedWaveInfo(const FDDWaveInfo WaveInfo);

private:

	void UpdateCastleStressBucket(float DeltaTime);
	void CheckGoldenEnemyAppearance();
	void CheckDisrupterAppearance();
	void CheckSpecialWave();
	void CalculatePlayerStyle();
	/*
	* For player style calculation
	* Only needs to be done once per wave
	*/
	FDDSecretStats CalculatePlayerUpgradeStats();
	FDDSecretStats CalculatePlaceablesStats();
	EDDPlayerStyle DeterminePlayerStyle(FVector Stats);
	/*
	*/

	/*
	* For enemy pool size calculation
	*/
	void CalculateWellnessPoints();
	void CalculateEnemyPoolSize();
	/*
	*/
	void DetermineEnemyPool();
	//Returns a pair of basic enemies number (first) and special enemies number (second)
	//Determines by choosing at random, with algorithms in place for how high of a chance
	//basic and special enemies can get chosen.
	TPair<int32, int32> DetermineEnemyTypesInPool() const;
	/// <summary>
	/// Determines enemies to be put in the enemy pool through varios weights and calculations.
	/// Just look at the function for more specific comments dawg
	/// </summary>
	/// <param name="EnemiesToSpawn"></param>
	/// <param name="EnemyArray">The types of enemies that could potentially be put in the enemy pool</param>
	/// <param name="EnemyAmount">Amount of enemies to be put in enemy pool</param>
	/// <returns>Array of enemies that have not been chosen for the enemy pool</returns>
	TArray<TSubclassOf<AEnemy>> DetermineEnemies(TArray<TSubclassOf<AEnemy>>& EnemiesToSpawn, 
												const TArray<TSubclassOf<AEnemy>> EnemyArray, 
												int32 EnemyAmount) const;
	void CalculateEnemyAmount();
	void CheckWildCardAppearance();
	void PityPointRewards();
	void ResetWaveInfo();
	//Resets all the temporary variables that help with draining the stress bucket (does not reset the bucket level itself)
	void ResetTempCastleStressBucketVariables();

	void InitializeOccupationSquares();
	void FetchOccupationSquareStats();
	void InitializeStressMeter();
	//Calculates threat levels per column of occupation squares
	void CalculateThreatLevelsPerColumn();
	void ResolveDowngradeFunctions();
	//Used for debugging purposes, force an enemy to always be added through the EnemiesToAlwaysAdd variable
	void AddEnemiesToAlwaysAdd();
	void ResetVariables();

	UFUNCTION()
	void OnStressMeterChange(EDDStressMeterTypes StressType);
	UFUNCTION()
	void OnSquareDetectEnemyDeath(FVector2D SquareCoordinate, AEnemy* Enemy);
	UFUNCTION()
	void OnThreatLevelChange(FVector2D SquareCoordinate, float OldThreatLevel);
	//Determines which column to set an enemy's location if they are a strategic enemy
	UFUNCTION()
	void OnStrategicEnemySpawn(AEnemy* Enemy);
	UFUNCTION()
	void OnPlayerUpgradedStat();
	UFUNCTION()
	void GameStartEventFunction();
	UFUNCTION()
	void WaveOverEventFunction();
	UFUNCTION()
	void NegativeSoulShopClosedEventFunction();
	UFUNCTION()
	void OnStartWaveFromBigShop();
	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void GameWaveJumpChoiceEventFunction();

	//Cached calculated player stats from when their upgrades' secret stats were added up
	FDDSecretStats CachedPlayerSecretStatsFromStats;

	TMap<EDDEnemyIDs, TSubclassOf<AEnemy>> EnemyEncyclopediaRef;
	TArray<TSubclassOf<AEnemy>> BasicEnemyArray;
	TArray<TSubclassOf<AEnemy>> SpecialEnemyArray;
	TArray<TSubclassOf<AEnemy>> AttackingSpecialEnemyArray;
	//Used for if a wild card wave occurs
	TArray<TSubclassOf<AEnemy>> SpecialEnemyLeftovers;
	ADDGameModeBase* GameMode;
	ADDEnemySpawner* EnemySpawner;
	ADDSpawnRangeLimiterManager* SpawnRangeLimiterManager;
	ADDPlaceableManager* PlaceableManager;
	ADDNegativeSoulShopManager* NegativeSoulShopManager;
	ADDPlayer* Player;
	UDDSoulShopWidget* SoulShopWidget;

	//Lower left corner (0, 0)
	//Lower right corner (0, Y)
	//Upper left corner (X, 0)
	//Upper right corner (X, Y)
	TArray<TArray<ADDOccupationSquare*>> OccupationSquares;
	//Summarizes each column of occupation squares' threat level
	TArray<float> ThreatLevelPerColumn;
	//Enemy that last died from an occupation square
	TWeakObjectPtr<AEnemy> LastEnemyDetectedOnDeath;
	//Downgrade functions to be resolved at the end of The King's calculations, if any
	TArray<std::function<void()>> DowngradeFunctions;

	float OriginalGoldenEnemyChance;
	//Minimum stress threshold to be added from the pity point rewards, if any
	float MinStressThresholdReward = 0.0f;

	bool bIsDisrupterHere = false;

	float CurrentTotalUpgradeThreatLevel;

	//For the castle stress bucket
	bool bIsTempBucketLevelSet = false;
	bool bHasStressLevelChanged = false;
	float TempCastleStressBucketDrainTime = 0.0f;
	float TempCastleStressBucketLevel = 0.0f;
	float TempStressLevelWithoutBucket = 0.0f;
	float TempCurrentStressLevel = 0.0f;
	float BucketDrainTimer = 0.0f;
	FTimerHandle BucketDrainDelayHandle;

	bool bWaveJumped = false;
	int32 WaveJumpedTotalEnemies;
};
