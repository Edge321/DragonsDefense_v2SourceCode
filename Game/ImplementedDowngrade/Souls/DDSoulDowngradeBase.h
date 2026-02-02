// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragonsDefense_v2/Game/DDDowngradeTypeInterface.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DDSoulDowngradeBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API UDDSoulDowngradeBase : public UObject, public IDDDowngradeTypeInterface
{
	GENERATED_BODY()
	
};
