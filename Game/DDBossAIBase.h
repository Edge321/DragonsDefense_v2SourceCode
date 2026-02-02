// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragonsDefense_v2/Characters/Bosses/DDBossEnemyBase.h"
#include "DDBossAIBase.generated.h"

/**
 * Interface in spirit, as doing an actual Interface for my Boss AI 
 * purposes does not work out for the code and blueprints
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API UDDBossAIBase : public UObject
{
	GENERATED_BODY()

public:
	//should be called on a tick basis
	virtual void DoBossAI(ADDBossEnemyBase* Boss) PURE_VIRTUAL(UDDBossAIBase::DoBossAI, );

	virtual void DoBossDeath(ADDBossEnemyBase* Boss) PURE_VIRTUAL(UDDBossAIBase::DoBossDeath, );
};
