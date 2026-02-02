// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaBase.h"
#include "DDTimeBasedDebuffAreaBase.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDTimeBasedDebuffAreaBase : public ADDDebuffAreaBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffLength = 3.0f;

	virtual void AddDebuffPair(ALivingActor* Living) override;

};
