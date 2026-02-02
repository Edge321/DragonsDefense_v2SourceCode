// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/Debuffs/DDTimeBasedDebuffBase.h"
#include "DDSlowDebuff.generated.h"

class AEnemy;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDSlowDebuff : public UDDTimeBasedDebuffBase
{
	GENERATED_BODY()

public:

	void ApplyDebuff(ALivingActor* Living) override;
	void RemoveDebuff() override;
	
private:

	TWeakObjectPtr<AEnemy> EnemyRef;

};
