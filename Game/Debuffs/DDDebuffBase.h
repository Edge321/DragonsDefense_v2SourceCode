// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragonsDefense_v2/Characters/LivingActor.h"
#include "DDDebuffBase.generated.h"

DECLARE_DELEGATE_OneParam(FOnRemoveDebuff, UDDDebuffBase*);

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDDebuffBase : public UObject
{
	GENERATED_BODY()

protected:

	float DebuffAmount = 1;
	TWeakObjectPtr<ALivingActor> Target;

	void PrintWrongActorError() const;

public:

	void SetDebuffAmount(const float NewDebuffAmount);

	virtual void ApplyDebuff(ALivingActor* Living) PURE_VIRTUAL(UDDDebuffBase::ApplyDebuff, );
	virtual void RemoveDebuff() PURE_VIRTUAL(UDDDebuffBase::RemoveDebuff, );

	FOnRemoveDebuff OnRemoveDebuff;

};
