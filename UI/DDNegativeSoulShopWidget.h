// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDNegativeSoulShopWidget.generated.h"

class ADDNegativeSoulShopManager;
class UDDDowngrade;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseNegativeShop);

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDNegativeSoulShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	//What wave does the Negative Soul Shop start to appear?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	int32 NegativeSoulShopWaveAppearance = 5;

	UPROPERTY(BlueprintReadOnly, Category = "Downgrade")
	TArray<UDDDowngrade*> CurrentDowngrades;
	UPROPERTY(BlueprintReadOnly)
	ADDNegativeSoulShopManager* Manager;
	UPROPERTY(BlueprintReadOnly)
	UDDDowngrade* CurrentActiveDowngrade;
	UPROPERTY(BlueprintReadOnly)
	bool bIsDowngradeActive = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsRewardActive = false;

	UFUNCTION(BlueprintCallable)
	void ValidateManager();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayDowngrades();
	UFUNCTION(BlueprintCallable)
	void RelayDowngrade(UDDDowngrade* Downgrade);
	UFUNCTION(BlueprintCallable)
	void BroadcastNegativeShopClose() const;
	UFUNCTION(BlueprintPure)
	const FString GetDowngradeName(const UDDDowngrade* Downgrade) const;
	UFUNCTION(BlueprintPure)
	const FString GetDowngradeDescription(const UDDDowngrade* Downgrade) const;
	UFUNCTION(BlueprintPure)
	const FString GetDowngradeRewardDescription(const UDDDowngrade* Downgrade) const;
	UFUNCTION(BlueprintCallable)
	void SetDowngradeSelection(const int32 DowngradeSelected);
	UFUNCTION(BlueprintPure)
	const int32 GetDowngradeSelection() const;
	UFUNCTION(BlueprintCallable)
	void ResetDowngradeSelection();

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction))
	const bool GetIsNegativeSoulShopActive() const;

	void SetManager(ADDNegativeSoulShopManager* NewManager);
	void SetDowngrades(TArray<UDDDowngrade*> Downgrades);
	void SetIsDowngradeActive(const bool Active);
	void SetIsRewardActive(const bool Active);
	//Request a downgrade that is currently selected. Returns nullptr if none selected
	void RequestDowngrade();

	UPROPERTY(BlueprintAssignable)
	FOnCloseNegativeShop OnCloseNegativeShop;

private:

	//Variable for Downgrade selection
	int32 CurrentSelection = -1;

};
