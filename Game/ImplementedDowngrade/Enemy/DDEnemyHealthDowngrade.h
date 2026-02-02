// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/ImplementedDowngrade/Enemy/DDEnemyDowngradeBase.h"
#include "DDEnemyHealthDowngrade.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDEnemyHealthDowngrade : public UDDEnemyDowngradeBase
{
	GENERATED_BODY()
	
public:

	void ApplyDowngrade(AActor* Target, const float DowngradeAmount) const override;

};
