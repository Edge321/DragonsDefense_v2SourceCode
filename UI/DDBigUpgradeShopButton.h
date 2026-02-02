// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDUpgradeShopButton.h"
#include "DDBigUpgradeShopButton.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDBigUpgradeShopButton : public UDDUpgradeShopButton
{
	GENERATED_BODY()

protected:

	virtual void OnClickEventFunction() override;

public:

	virtual void InitializeButton() override;
	virtual bool TryBuy() override;

};
