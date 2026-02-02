// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDSoulShopWidget.generated.h"

class UDDShopButton;
class UDDUpgradeShopButton;
class UCanvasPanel;

enum class EPlayerStats : uint8;
enum class EDDStressMeterTypes : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickBigShopButton);
DECLARE_DELEGATE(FOnStartWaveFromBigShop);
DECLARE_DELEGATE(FOnRequestShowAllPlaceableInfo)
DECLARE_DELEGATE(FOnRequestHideAllPlaceableInfo)
DECLARE_DELEGATE_OneParam(FOnPlayerUpgrade, EDDStressMeterTypes);

USTRUCT(BlueprintType)
struct FDDButtonStruct {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EPlayerStats Stat;
	UPROPERTY(BlueprintReadOnly)
	int32 UpgradeAmount;
	UPROPERTY(BlueprintReadOnly)
	int32 UpgradeTotal;
};

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDSoulShopWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Icons | Upgrade Icons")
	TSubclassOf<UTexture2D> DamageIcon;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPlaceableInfoHidden = true;

	UFUNCTION(BlueprintCallable)
	void SoulShopWidgetInitialization();
	UFUNCTION(BlueprintCallable)
	TArray<UDDShopButton*> FindAllShopButtons(UCanvasPanel* Canvas);
	UFUNCTION(BlueprintCallable)
	void UpdateButtonsOfSameCategory(UDDUpgradeShopButton* Button);
	UFUNCTION(BlueprintCallable)
	void BroadcastCollapseBigShop() const;
	UFUNCTION(BlueprintCallable)
	void ExecuteOnStartWaveFromBigShopEvent() const;
	UFUNCTION(BlueprintCallable)
	void ExecuteOnRequestShowAllPlaceableInfo() const;
	UFUNCTION(BlueprintCallable)
	void ExecuteOnRequestHideAllPlaceableInfo() const;
	/// <summary>
	/// Switches the active button of the shop
	/// </summary>
	/// <param name="SrcButton">Button that will turn inactive</param>
	/// <param name="DestButton">Button that will turn active</param>
	UFUNCTION(BlueprintCallable)
	void SwitchActiveShopButton(UButton* SrcButton, UButton* DestButton);
	/// <summary>
	/// Switches the active button of the shop using the stored active shop button
	/// </summary>
	/// <param name="SrcButton"></param>
	/// <param name="DestButton"></param>
	UFUNCTION(BlueprintCallable)
	void SwitchActiveShopButtonWithoutSrc(UButton* DestButton);
	UFUNCTION(BlueprintCallable)
	void StoreActiveShopButton(UButton* Button);
	UFUNCTION(BlueprintCallable)
	void SwitchActiveShopTab(UWidget* DestWidget);
	UFUNCTION(BlueprintCallable)
	void StoreActiveShopTab(UWidget* Widget);
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetPlaceableInfoVisibilityEventFunction(const bool bPlaceableInfoVisibility);

public:

	//For blueprints since TArrays in TMap are not allowed with GetUpgradeButtonsByCategory
	UFUNCTION(BlueprintCallable)
	const TArray<FDDButtonStruct> GetUpgradeButtonsArray();
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void UpdateSoulsText(int32 Souls);
	UFUNCTION(BlueprintImplementableEvent, Category = "Event")
	void LoadUnlockedButtons(int32 WaveNumber);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (ForceAsFunction))
	void FlipArrowImage();
	//Returns true is the mouse is currently hovering over the big or small soul shop
	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction))
	const bool GetIsMouseHoveringWidget() const;

	UPROPERTY(BlueprintAssignable)
	FOnClickBigShopButton OnClickBigShopButton;

	FOnStartWaveFromBigShop	OnStartWaveFromBigShop;
	FOnRequestShowAllPlaceableInfo OnRequestShowAllPlaceableInfo;
	FOnRequestHideAllPlaceableInfo OnRequestHideAllPlaceableInfo;

	const TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> GetUpgradeButtonsByCategory() const;

	FOnPlayerUpgrade OnPlayerUpgrade;

private:

	void AddToButtonMap(UDDUpgradeShopButton* Button);
	void RecursiveFindButtons(UWidget* Widget, TArray<UDDShopButton*>& ButtonArray);

	UFUNCTION()
	void GameStartEventFunction();
	
	TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> UpgradeButtonsByCategory;

	TWeakObjectPtr<UButton> ActiveButton;
	TWeakObjectPtr<UWidget> ActiveBigScrollBox;
};
