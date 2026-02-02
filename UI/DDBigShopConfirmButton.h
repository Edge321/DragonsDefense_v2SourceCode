// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DDBigShopConfirmButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuy);

class UDDShopButton;

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

public:

	UFUNCTION(BlueprintCallable)
	void InitializeButton();
	UFUNCTION(BlueprintCallable)
	UDDShopButton* GetCurrentBigShopButtonPtr();

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
};
