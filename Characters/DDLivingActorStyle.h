// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"

/**
 * Enum that defines if a living actor is seen as the following types
 * based on their secret stats
 */
UENUM(BlueprintType)
enum class EDDLivingActorStyle : uint8
{
	Aggressive,
	Defensive,
	Supportive,
	Generalist
};
