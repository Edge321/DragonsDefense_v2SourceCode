// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDowngradeCategories : uint8 {
	Player,
	Enemy,
	Wave,
	Placeables,
	Sentients,
	Traps,
	Souls,
	TheKing
};
