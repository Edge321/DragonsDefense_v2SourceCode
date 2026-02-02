// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDBossAIBase.h"
#include "DDRoyalHeirBossAIBase.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDRoyalHeirBossAIBase : public UDDBossAIBase
{
	GENERATED_BODY()

public:

	virtual void DoBossAI(ADDBossEnemyBase* Boss) override;

	virtual void DoBossDeath(ADDBossEnemyBase* Boss) override;

};
