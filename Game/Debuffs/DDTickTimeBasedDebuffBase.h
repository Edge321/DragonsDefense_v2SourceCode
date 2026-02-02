// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/Debuffs/DDTickBasedDebuffBase.h"
#include "DragonsDefense_v2/Game/Debuffs/DDTimeBasedDebuffBase.h"
#include "DDTickTimeBasedDebuffBase.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDTickTimeBasedDebuffBase : public UDDTickBasedDebuffBase
{
	GENERATED_BODY()
	
protected:

	float DebuffLength = 5;
	FTimerHandle DebuffTimer;

public:

	virtual void ApplyDebuff(ALivingActor* Living) override;
	virtual void RemoveDebuff() override;
	void SetDebuffLength(const float NewDebuffLength);

private:

	void StartTimer();

};
