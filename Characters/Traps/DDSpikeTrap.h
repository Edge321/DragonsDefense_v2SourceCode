// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Traps/DDDebuffAreaTrap.h"
#include "DDSpikeTrap.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDSpikeTrap : public ADDDebuffAreaTrap
{
	GENERATED_BODY()

protected:
	
	void EnableDebuffArea() override;
	void DisableDebuffArea() override;
	
};
