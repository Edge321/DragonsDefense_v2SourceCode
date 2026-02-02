// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/ImplementedDowngrade/Player/DDPlayerDowngradeBase.h"
#include "DDPlayerSpeedDowngrade.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPlayerSpeedDowngrade : public UDDPlayerDowngradeBase
{
	GENERATED_BODY()
	
public:

	void ApplyDowngrade(AActor* Target, const float DowngradeAmount) const override;

};
