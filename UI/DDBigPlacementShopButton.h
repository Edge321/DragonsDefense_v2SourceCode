// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDPlacementShopButton.h"
#include "DDBigPlacementShopButton.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDBigPlacementShopButton : public UDDPlacementShopButton
{
	GENERATED_BODY()
	
protected:

	virtual void OnClickEventFunction() override;

public:

	virtual bool TryBuy() override;

private:

	void InitializeDescription();

	bool bHasDescriptionInitialized = false;

};
