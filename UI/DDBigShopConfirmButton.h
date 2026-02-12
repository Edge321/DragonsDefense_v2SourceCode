// Copyright (c) 2026, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DDBigShopConfirmButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuy);

class UDDShopButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDBigShopConfirmButton : public UButton
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere)
	UTextBlock* ButtonText;
	UPROPERTY(EditDefaultsOnly, Category = "Buy Color")
	FLinearColor UnbuyableColor = FLinearColor::Red;

public:

	UFUNCTION(BlueprintCallable)
	void InitializeButton();
	UFUNCTION(BlueprintCallable)
	UDDShopButton* GetCurrentBigShopButtonPtr();
	//Updates the confirm button by checking if the item its viewing is still in a buyable state
	UFUNCTION(BlueprintCallable)
	void UpdateBigConfirmButton();

	UPROPERTY(BlueprintAssignable)
	FOnBuy OnBuy;

protected:
	
	UFUNCTION(BlueprintCallable)
	void OnBigShopButtonClicked(UDDShopButton* Button);
	
	UFUNCTION()
	void OnClickEventFunction();
	UFUNCTION()
	void WaveStartEventFunction();
	UFUNCTION()
	void GameOverEventFunction();
	

private:

	TWeakObjectPtr<UDDShopButton> BigShopButtonPtr;

	FLinearColor OriginalButtonColor;
};
