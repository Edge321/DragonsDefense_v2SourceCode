// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDDStressMeterTypes : uint8 {
	EnemySpawn,
	EnemyDeath,
	CastleAttacked,
	PlaceableDeath,
	PlaceableSpawn,
	PlayerUpgrade
};
