// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDPlaceableStateInterface.h"
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"
#include "DDSentientStateInterface.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API UDDSentientStateInterface : public UObject, public IDDPlaceableStateInterface
{
	GENERATED_BODY()

public:

	void SetOwner(AActor* Actor) override;

protected:

	TWeakObjectPtr<ADDSentientPlaceable> SentientOwner;

};
