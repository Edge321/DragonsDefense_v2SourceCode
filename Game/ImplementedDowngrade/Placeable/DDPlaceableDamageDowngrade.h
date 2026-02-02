// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragonsDefense_v2/Game/ImplementedDowngrade/Placeable/DDPlaceableDowngradeBase.h"
#include "DDPlaceableDamageDowngrade.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPlaceableDamageDowngrade : public UDDPlaceableDowngradeBase
{
	GENERATED_BODY()
	
public:

	void ApplyDowngrade(AActor* Target, const float DowngradeAmount) const override;

};
