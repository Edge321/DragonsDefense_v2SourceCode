// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlayerStats : uint8 {
	MovementSpeed,
	ShootSpeed,
	MaxHealth,
	Health,
	Damage,
	ProjectileSpeed,
	Penetration,
	CriticalChance,
	ProjectileSize,
	LineOfSight,
	Foresight
};

//This might be a terrible way to do this, ngl
extern const TArray<FString> PlayerStatStrings;