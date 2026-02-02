// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDInfiniteUpgradeButton.h"
#include "DDBigInfiniteUpgradeButton.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDBigInfiniteUpgradeButton : public UDDInfiniteUpgradeButton
{
	GENERATED_BODY()
	
protected:

	virtual void OnClickEventFunction() override;

public:

	virtual bool TryBuy() override;

};
