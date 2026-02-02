// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DDDowngradeTypeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDDDowngradeTypeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DRAGONSDEFENSE_V2_API IDDDowngradeTypeInterface
{
	GENERATED_BODY()

public:

	virtual void ApplyDowngrade(AActor* Target, const float DowngradeAmount) const PURE_VIRTUAL(IDDDowngradeTypeInterface::ApplyDowngrade,);

protected:

	void PrintErrorLog() const;

};
