// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDPlayerStatisticsWidget.generated.h"

USTRUCT(BlueprintType)
struct FUpgradeInfo {

	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentUpgrade;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxUpgrade;
	UPROPERTY(BlueprintReadOnly)
	FString UpgradeName;
	UPROPERTY(BlueprintReadOnly)
	FString UpgradeRank;

};

class ADDGameModeBase;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPlayerStatisticsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintPure)
	const TArray<FUpgradeInfo> GetUpgradeInfoArray() const;
	UFUNCTION(BlueprintCallable)
	void UpdateUpgradeInfoArray();
	UFUNCTION(BlueprintCallable)
	void PopulateUpgradeInfoArray();

private:

	TArray<FUpgradeInfo> UpgradeInfoArray;

	TWeakObjectPtr<ADDGameModeBase> GameModeRef;

};
