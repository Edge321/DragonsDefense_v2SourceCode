// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/UI/DDInfiniteShopButton.h"
#include "DDInfiniteUpgradeButton.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDInfiniteUpgradeButton : public UDDInfiniteShopButton
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category = "Upgrade|UpgradeAmount")
	float UpgradeAmount = 1;
	UPROPERTY(EditAnywhere, Category = "Upgrade")
	EPlayerStats UpgradeStat = EPlayerStats::Health;

	void Upgrade(EPlayerStats Stat);
	void OnHoveredEventFunction() override;
	void OnClickEventFunction() override;

};
