// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDPlayerStatInfo.h"

const FString FDDPlayerStatInfo::GetFuzzyStat(const EPlayerStats PlayerStat, const float Stat) const
{
	FPlayerInfo PlayerInfo;

	if (AllPlayerInfo.Contains(PlayerStat)) {
		PlayerInfo = AllPlayerInfo[PlayerStat];
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Unable to find PlayerStat %s"), *PlayerStatStrings[(int32) PlayerStat])
		return "";
	}

	for (TPair<float, FString> FuzzyPair : PlayerInfo.FuzzyMap) {
		if (PlayerInfo.bIsAscending) {
			if (Stat < FuzzyPair.Key) {
				return FuzzyPair.Value;
			}
		}
		else {
			if (Stat > FuzzyPair.Key) {
				return FuzzyPair.Value;
			}
		}
	}

	return PlayerInfo.MaxStat;
}
