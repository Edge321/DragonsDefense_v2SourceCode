// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlaceableAI : uint8 {
	Deactivated,
	ClosestEnemy,
	FurthestEnemy,
	ClosestToCastle,
	FurthestFromCastle,
	RoundRobin,
	Random
};