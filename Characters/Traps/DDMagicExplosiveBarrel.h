// Copyright (c) 2026, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDTrapPlaceable.h"
#include "DDMagicExplosiveBarrel.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDMagicExplosiveBarrel : public ADDTrapPlaceable
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

};
