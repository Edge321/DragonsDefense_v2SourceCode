// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDTimeBasedDebuffBase.h"
#include "DDPermanentSlowDebuff.generated.h"

/**
 * Permanently applies a slow debuff, ignoring any resistance the LivingActor may have
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPermanentSlowDebuff : public UDDTimeBasedDebuffBase
{
	GENERATED_BODY()
	
public:

	virtual void ApplyDebuff(ALivingActor* Living) override;
	virtual void RemoveDebuff() override;

};
