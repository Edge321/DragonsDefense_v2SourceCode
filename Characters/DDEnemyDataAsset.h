// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DDEnemyDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDEnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	//A unique ID for the current enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 EnemyID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FString EnemyName = "Villager";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FString EnemyDescription = "Description";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Damage = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float MovementSpeed = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 SoulValue = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FString SpecialAbilityDescription = "None";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FString MaxFuzzyDescription = "SSS";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	UTexture2D* EnemyImage;

	UPROPERTY(EditAnywhere, Category = "Stats|Description")
	bool bOverrideDamageDescription = false;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Description", meta = (EditCondition = "bOverrideDamageDescription", EditConditionHides))
	FString DamageDescription = "Hurty";
	UPROPERTY(EditAnywhere, Category = "Stats|Description")
	bool bOverrideHealthDescription = false;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Description", meta = (EditCondition = "bOverrideHealthDescription", EditConditionHides))
	FString HealthDescription = "Healthy";
	UPROPERTY(EditAnywhere, Category = "Stats|Description")
	bool bOverrideAttackSpeedDescription = false;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Description", meta = (EditCondition = "bOverrideAttackSpeedDescription", EditConditionHides))
	FString AttackSpeedDescription = "Attack speedy!";
	UPROPERTY(EditAnywhere, Category = "Stats|Description")
	bool bOverrideMovementSpeedDescription = false;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Description", meta = (EditCondition = "bOverrideMovementSpeedDescription", EditConditionHides))
	FString MovementSpeedDescription = "I can see everything!";
	UPROPERTY(EditAnywhere, Category = "Stats|Description")
	bool bOverrideSoulValueDescription = false;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Description", meta = (EditCondition = "bOverrideSoulValueDescription", EditConditionHides))
	FString SoulValueDescription = "I can see everything!";

	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|Damage")
	TMap<float, FString> FuzzyDamage = {
		{0.5f, "F"},
		{0.75f, "D"},
		{1.0f, "C"},
		{1.5f, "B"},
		{3.0f, "A"},
		{5.5f, "S"}
	};
	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|Health")
	TMap<float, FString> FuzzyHealth = {
		{3.0f, "F"},
		{5.0f, "D"},
		{6.0f, "C"},
		{7.5f, "B"},
		{10.0f, "A"},
		{15.0f, "S"}
	};
	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|AttackSpeed")
	TMap<float, FString> FuzzyAttackSpeed = {
		{4.0f, "F"},
		{3.0f, "D"},
		{2.5f, "C"},
		{1.75f, "B"},
		{1.4f, "A"},
		{1.0f, "S"}
	};
	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|MovementSpeed")
	TMap<float, FString> FuzzyMovementSpeed = {
		{30.0f, "F"},
		{40.0f, "D"},
		{50.0f, "C"},
		{60.0f, "B"},
		{70.0f, "A"},
		{110.0f, "S"}
	};
	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|SoulValue")
	TMap<int32, FString> FuzzySoulValue = {
		{0, "F"},
		{1, "D"},
		{2, "C"},
		{3, "B"},
		{4, "A"},
		{5, "S"}
	};

	UFUNCTION(BlueprintPure)
	const FString GetDamageFuzzification() const;
	UFUNCTION(BlueprintPure)
	const FString GetHealthFuzzification() const;
	UFUNCTION(BlueprintPure)
	const FString GetAttackSpeedFuzzification() const;
	UFUNCTION(BlueprintPure)
	const FString GetMovementSpeedFuzzificaiton() const;
	UFUNCTION(BlueprintPure)
	const FString GetSoulValueFuzzification() const;

};
