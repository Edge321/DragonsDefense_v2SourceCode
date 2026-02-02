// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDPlayerStats.h"
#include "DDPlayerStatInfo.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	bool bIsAscending = true;
	UPROPERTY(EditDefaultsOnly)
	TMap<float, FString> FuzzyMap;
	UPROPERTY(EditDefaultsOnly)
	FString MaxStat = "SSS";

};

/**
 * 
 */
USTRUCT(BlueprintType)
struct DRAGONSDEFENSE_V2_API FDDPlayerStatInfo
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	TMap<EPlayerStats, FPlayerInfo> AllPlayerInfo;

public:
	
	const FString GetFuzzyStat(const EPlayerStats PlayerStat, const float Stat) const;

};
