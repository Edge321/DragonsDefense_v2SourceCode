// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DDPlaceableDataAsset.generated.h"

class UImage;

/**
 * 
 */
UCLASS(BlueprintType)
class DRAGONSDEFENSE_V2_API UDDPlaceableDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FString PlaceableName = "Placeable";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	FString PlaceableDescription = "Description";
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	UTexture2D* PlaceableImage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Damage = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Health = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackSpeed = -1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackRadius = -1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Default")
	FString MaxFuzzyDescription = "SSS";

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
	bool bOverrideAttackRadiusDescription = false;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Description", meta = (EditCondition = "bOverrideAttackRadiusDescription", EditConditionHides))
	FString AttackRadiusDescription = "I can see everything!";

	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|Damage")
	TMap<float, FString> FuzzyDamage = {
		{0.25f, "F"},
		{0.50f, "D"},
		{0.75f, "C"},
		{1.0f, "B"},
		{2.0f, "A"},
		{6.0f, "S"}
	};
	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|Health")
	TMap<float, FString> FuzzyHealth = {
		{2.0f, "F"},
		{3.0f, "D"},
		{4.0f, "C"},
		{6.0f, "B"},
		{7.0f, "A"},
		{9.0f, "S"}
	};
	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|AttackSpeed")
	TMap<float, FString> FuzzyAttackSpeed = {
		{5.0f, "F"},
		{4.0f, "D"},
		{3.0f, "C"},
		{1.5f, "B"},
		{1.0f, "A"},
		{0.5f, "S"}
	};
	UPROPERTY(VisibleDefaultsOnly, Category = "Stats|AttackRadius")
	TMap<float, FString> FuzzyAttackRadius = {
		{250.0f, "F"},
		{300.0f, "D"},
		{350.0f, "C"},
		{400.0f, "B"},
		{450.0f, "A"},
		{650.0f, "S"}
	};
	
	UFUNCTION(BlueprintPure)
	const FString GetDamageFuzzification() const;
	UFUNCTION(BlueprintPure)
	const FString GetHealthFuzzification() const;
	UFUNCTION(BlueprintPure)
	const FString GetAttackSpeedFuzzification() const;
	UFUNCTION(BlueprintPure)
	const FString GetAttackRadiusFuzzificaiton() const;

};
