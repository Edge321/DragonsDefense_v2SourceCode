// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/Debuffs/DDTickTimeBasedDebuffBase.h"
#include "DDDamageTickTimeDebuff.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDDamageTickTimeDebuff : public UDDTickTimeBasedDebuffBase
{
	GENERATED_BODY()
	
public:

	void ApplyDebuff(ALivingActor* Living) override;
	void UpdateDebuff(float DeltaTime) override;

};
