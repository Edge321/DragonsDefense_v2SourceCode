// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDTrapPlaceable.h"
#include "DDMagicMineTrap.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDMagicMineTrap : public ADDTrapPlaceable
{
	GENERATED_BODY()

protected:

	void HandleBeginOverlap(AActor* Actor) override;

public:

	void ConvertToEvil() override;
	
private:

	bool bIsAboutToExplode = false;

};
