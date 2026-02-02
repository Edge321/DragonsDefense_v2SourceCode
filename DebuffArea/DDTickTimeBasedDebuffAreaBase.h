// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaBase.h"
#include "DDTickTimeBasedDebuffAreaBase.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDTickTimeBasedDebuffAreaBase : public ADDDebuffAreaBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffTickInterval = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffLength = 3.0f;

	virtual void AddDebuffPair(ALivingActor* Living) override;

};
