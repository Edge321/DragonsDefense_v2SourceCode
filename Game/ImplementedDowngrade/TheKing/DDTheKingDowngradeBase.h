// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragonsDefense_v2/Game/DDDowngradeTypeInterface.h"
#include "DragonsDefense_v2/Game/Managers/DDTheKing.h"
#include "DDTheKingDowngradeBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API UDDTheKingDowngradeBase : public UObject, public IDDDowngradeTypeInterface
{
	GENERATED_BODY()
	
};
