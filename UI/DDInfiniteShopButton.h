// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/UI/DDShopButton.h"
#include "DDInfiniteShopButton.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API UDDInfiniteShopButton : public UDDShopButton
{
	GENERATED_BODY()

public:

	virtual const int32 GetCurrentPrice() const override; //BlueprintPure
	virtual const FText FetchFTextPrice() const override; //BlueprintPure

protected:

	UFUNCTION(BlueprintCallable)
	void DisableButton();
	UFUNCTION(BlueprintCallable)
	void EnableButton();

	UPROPERTY(EditAnywhere, Category = "Price")
	int32 Price = 0;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsAuthorized = true;

	virtual void UpdateSouls() override;
	virtual void UpdateText() override;
	virtual void IsBuyable() override;
	

};
