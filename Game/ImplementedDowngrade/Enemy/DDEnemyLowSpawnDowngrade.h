// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/ImplementedDowngrade/Enemy/DDEnemyDowngradeBase.h"
#include "DDEnemyLowSpawnDowngrade.generated.h"

/**
 * Sets the low spawn interval mod
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDEnemyLowSpawnDowngrade : public UDDEnemyDowngradeBase
{
	GENERATED_BODY()
	
public:

	void ApplyDowngrade(AActor* Target, const float DowngradeAmount) const override;

};
