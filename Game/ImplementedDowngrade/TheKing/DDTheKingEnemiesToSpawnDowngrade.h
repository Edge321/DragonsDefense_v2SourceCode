// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDTheKingDowngradeBase.h"
#include "DDTheKingEnemiesToSpawnDowngrade.generated.h"

/**
 * Uses DowngradeAmount to determine Enemy ID, instead of the usual Downgrade Amount multiplier
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDTheKingEnemiesToSpawnDowngrade : public UDDTheKingDowngradeBase
{
	GENERATED_BODY()

public:

	void ApplyDowngrade(AActor* Target, const float DowngradeAmount) const override;
	
private:

	void DowngradeFunction() const;

};
