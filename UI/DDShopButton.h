// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "DragonsDefense_v2/Characters/DDPlayerStats.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/UI/DDLockButton.h"
#include "DDShopButton.generated.h"

class ADDPlaceable;
class UBorder;
class UDDLockButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlaceableHovered, UDDShopButton*, Button, TSubclassOf<ADDPlaceable>, PlaceableClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeHovered, UDDShopButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockButtonHovered, UDDShopButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBigShopButtonClicked, UDDShopButton*, Button, TSubclassOf<ADDPlaceable>, PlaceableClass);

/**
 * 
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API UDDShopButton : public UButton
{
	GENERATED_BODY()

protected:

	//Text for displaying the current price of the shop item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PriceText")
	UTextBlock* PriceText;
	UPROPERTY(EditAnywhere, Category = "Button Color")
	FLinearColor BuyableColor = FLinearColor::Blue;
	UPROPERTY(EditAnywhere, Category = "Button Color")
	FLinearColor UnBuyableColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Border Properties")
	FLinearColor BorderColor = FLinearColor::Green;
	UPROPERTY(EditDefaultsOnly, Category = "Border Properties")
	FMargin BorderEnabled = FMargin(5.0f, 5.0f, 5.0f, 5.0f);
	UPROPERTY(EditAnywhere, Category = "Description")
	FString ButtonDescription = "Insert Description";
	UPROPERTY(EditAnywhere, Category = "Upgrade")
	bool bIsBuyable = false;
	UPROPERTY(EditAnywhere, Category = "Upgrade")
	UTexture2D* ButtonIcon;
	UPROPERTY(EditAnywhere, Category = "Description")
	FString ButtonBuyText;
	//If the tooltip should be shown on hovering the button
	UPROPERTY(EditAnywhere, Category = "Tooltip")
	bool bTooltipEnabled = true;
	//How far up in the parent chain is the Overlay widget (Used for laying the lock image on top of the button)
	UPROPERTY(EditDefaultsOnly, Category = "Button Lock")
	int32 OverlaySearchAmount = 2;
	//How far up in the parent chain is the Border widget (Used for highlighting the button)
	UPROPERTY(EditDefaultsOnly, Category = "Button Lock")
	int32 BorderSearchAmount = 1;
	UPROPERTY(EditAnywhere, Category = "Button Lock")
	int32 WaveUnlocked = 1;
	UPROPERTY(EditDefaultsOnly, Category = "Button Lock")
	UTexture2D* LockedIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Button Lock")
	UTexture2D* UnlockedIcon;
	//Text block that contains the name of the shop button
	//e.g. Big shop buttons have their respective text block names
	UPROPERTY(EditAnywhere, Category = "Button Lock")
	UTextBlock* ItemNameText;

public:

	//Checks if the PriceText variable was set. Otherwise, this shit would be pointless!!!!
	UFUNCTION(BlueprintCallable)
	virtual void ValidatePriceText();
	UFUNCTION(BlueprintCallable)
	virtual void InitializeButton();
	UFUNCTION(BlueprintPure)
	const FString GetDescription() const;
	//Returns true if the button is buyable
	//Returns false if not
	//Does a pre-check of it is buyable before returning
	UFUNCTION(BlueprintPure)
	const bool GetBuyableStatus();
	UFUNCTION(BlueprintPure)
	const UTexture2D* GetButtonIcon() const;
	//Intended for the big confirm button
	UFUNCTION(BlueprintPure)
	const FString GetButtonBuyText() const;
	UFUNCTION(BlueprintPure)
	const bool GetTooltipEnabled() const;
	UFUNCTION(BlueprintPure)
	const int32 GetWaveUnlocked() const;

	UFUNCTION(BlueprintCallable)
	virtual void GameOverEventFunction();
	//Unlocks the button, removing the lock image
	UFUNCTION(BlueprintCallable)
	virtual void Unlock();
	//Locks the button, adding the lock image
	UFUNCTION(BlueprintCallable)
	virtual void Lock();
	//Unlocks the button, skipping the unlock animation
	UFUNCTION(BlueprintCallable)
	void InstantUnlock();
	UFUNCTION(BlueprintPure)
	virtual const int32 GetCurrentPrice() const { return 0; }
	UFUNCTION(BlueprintPure)
	virtual const FString GetCurrentPriceToString() const { return ""; }
	UFUNCTION(BlueprintPure)
	virtual const FText FetchFTextPrice() const { return FText(); }
	UFUNCTION(BlueprintCallable)
	virtual void IsBuyable() {}; //pure virtual
	UFUNCTION(BlueprintCallable)
	virtual bool TryBuy() { return false; }; //pure virtual, for Big Shop Buttons only
	
	void DeselectButton();

	UPROPERTY(BlueprintAssignable)
	FOnPlaceableHovered OnPlaceableHovered;
	UPROPERTY(BlueprintAssignable)
	FOnUpgradeHovered OnUpgradeHovered;
	UPROPERTY(BlueprintAssignable)
	FOnBigShopButtonClicked OnBigShopButtonClicked;
	UPROPERTY(BlueprintAssignable)
	FOnLockButtonHovered OnLockButtonHovered;

protected:

	UFUNCTION()
	virtual void OnClickEventFunction() {}; //pure virtual
	UFUNCTION()
	virtual void OnHoveredEventFunction() {}; //pure virtual
	UFUNCTION()
	void OnLockHoveredEventFunction();
	
	virtual void UpdateSouls() {}; //pure virtual
	virtual void UpdateText() {}; //pure virtual

	void EnableGreenBorder();
	void DisableGreenBorder();

	//Used for if the button has been selected
	UBorder* GreenBorderOutline;

	UDDLockButton* LockImageWidget;

private:

	UFUNCTION()
	void OnUnhoveredEventFunction();
	
	//Stores text of what the item button is, if there exists such text
	FString ActualItemNameString;
	//Stores text of what the price is
	FString ActualPriceString;

};
