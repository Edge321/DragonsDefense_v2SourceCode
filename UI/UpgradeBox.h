// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/HorizontalBox.h"
#include "UpgradeBox.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UUpgradeBox : public UHorizontalBox
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Upgrades")
	UTexture2D* UpgradeImage;
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades")
	UTexture2D* NonUpgradeImage;
	//Sets how big the Upgrade array is from the start
	UPROPERTY(EditDefaultsOnly, Category = "Upgrades")
	int32 UpgradeImagesSize = 10;
	UPROPERTY(EditAnywhere, Category = "Upgrades")
	FVector2D ImageSize = FVector2D(16.0f, 32.0f);

public:

	UFUNCTION(BlueprintCallable)
	void InitializeImages();
	UFUNCTION(BlueprintCallable)
	void SetUpgradeBox(const int32 CurrentUpgrade, const int32 MaxUpgrades);

private:

	//Creates a new image object and adds to the necessary components
	void NewUpgradeImage();

	UPROPERTY()
	TArray<UImage*> UpgradeArray;

};
