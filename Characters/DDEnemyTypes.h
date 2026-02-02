// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDEnemyTypes.generated.h"

/**
 * The type of a specific enemy
 */
UENUM(BlueprintType)
enum class EDDEnemyType : uint8
{
	Basic,
	Special
};
