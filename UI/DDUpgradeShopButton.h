// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/UI/DDShopButton.h"
#include "DragonsDefense_v2/Characters/DDSecretStats.h"
#include "DDUpgradeShopButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgrade, UDDUpgradeShopButton*, Button);

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDUpgradeShopButton : public UDDShopButton
{
	GENERATED_BODY()

protected:
	//An array of increasing prices for when the player purchases whatever the button sells
	UPROPERTY(EditAnywhere, Category = "Prices")
	TArray<int32> Prices;
	UPROPERTY(EditAnywhere, Category = "Upgrade|UpgradeAmount")
	float UpgradeAmount = 1;
	UPROPERTY(EditAnywhere, Category = "Upgrade")
	EPlayerStats StatUpgrade = EPlayerStats::Damage;
	UPROPERTY(EditAnywhere, Category = "Secret Stats")
	FDDSecretStats SecretStats;
	//Threat level per upgrade
	UPROPERTY(EditAnywhere, Category = "Secret Stats", meta = (ClampMin = "1.0", ClampMax = "100.0", UIMin = "1.0", UIMax = "100.0"))
	float ThreatLevel;

	virtual void IsBuyable() override;
	void Upgrade(EPlayerStats Stat) const;
	virtual void UpdateSouls() override;
	virtual void GameOverEventFunction() override;
	virtual void OnClickEventFunction() override;
	virtual void OnHoveredEventFunction() override;

public:
	
	virtual void InitializeButton() override;
	UFUNCTION(BlueprintPure)
	const EPlayerStats GetStatUpgrade() const;
	UFUNCTION(BlueprintPure)
	const FDDSecretStats GetSecretStats() const;
	UFUNCTION(BlueprintPure)
	const float GetThreatLevel() const;
	UFUNCTION(BlueprintPure)
	const float GetPrefixThreatLevel(int32 Index) const;
	//Where the player is currently at for upgrades
	UFUNCTION(BlueprintPure)
	const int32 GetPriceIndex() const;
	UFUNCTION(BlueprintPure)
	const int32 GetPrefixSumPrice(int32 Index) const;
	//Get the size of the amount of upgrades in the button
	UFUNCTION(BlueprintPure)
	const int32 GetPricesSize() const;
	UFUNCTION(BlueprintPure)
	const FString GetPlayerStatToString() const;
	UFUNCTION(BlueprintPure)
	const FString GetPlayerStatGrade() const;
	UFUNCTION(BlueprintCallable)
	void IncreasePrice();
	
	virtual const int32 GetCurrentPrice() const override; //BlueprintPure
	//Fetches the current price but is converted to FText
	virtual const FText FetchFTextPrice() const override; //BlueprintPure

	UPROPERTY(BlueprintAssignable)
	FOnUpgrade OnUpgrade;

private:
	void UpdateText() override;
	bool IsMaxxedOut();
	void ResetPrice();

	int32 PriceIndex = 0;

	TArray<int32> PrefixSumPrices;
	TArray<float> PrefixThreatLevels;
};
