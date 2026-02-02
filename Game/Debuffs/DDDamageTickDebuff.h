// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/Debuffs/DDTickBasedDebuffBase.h"
#include "DDDamageTickDebuff.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDDamageTickDebuff : public UDDTickBasedDebuffBase
{
	GENERATED_BODY()
	
public:

	void ApplyDebuff(ALivingActor* Living) override;
	void RemoveDebuff() override;
	void UpdateDebuff(float DeltaTime) override;

};
