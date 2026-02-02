// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/Debuffs/DDDebuffBase.h"
#include "DDTickBasedDebuffBase.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDTickBasedDebuffBase : public UDDDebuffBase
{
	GENERATED_BODY()
	
protected:

	float DebuffTickInterval = 1;
	float TotalTimePassed = 0;

public:

	void SetTickInterval(const float NewTickInterval);
	virtual void UpdateDebuff(float DeltaTime) PURE_VIRTUAL(UDDTickBasedDebuffBase::UpdateDebuff, );

};
