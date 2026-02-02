// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DDResettableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDDResettableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DRAGONSDEFENSE_V2_API IDDResettableInterface
{
	GENERATED_BODY()

public:

	//Resets any modifiers placed on object
	virtual void ResetMods() = 0;

};
