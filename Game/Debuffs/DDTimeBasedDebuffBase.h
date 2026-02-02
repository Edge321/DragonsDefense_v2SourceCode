// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/Debuffs/DDDebuffBase.h"
#include "DDTimeBasedDebuffBase.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDTimeBasedDebuffBase : public UDDDebuffBase
{
	GENERATED_BODY()
	
protected:

	float DebuffLength = 1;
	FTimerHandle DebuffTimer;

public:

	void SetDebuffLength(const float NewDebuffLength);
	void StartDebuff();

};
