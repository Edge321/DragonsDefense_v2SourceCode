// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDSoulDowngradeBase.h"
#include "DDSoulPermanentDowngrade.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDSoulPermanentDowngrade : public UDDSoulDowngradeBase
{
	GENERATED_BODY()

public:

	virtual void ApplyDowngrade(AActor* Target, const float DowngradeAmount) const override;

};
