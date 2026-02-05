// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DDSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UDDSaveGame();

public:

	//Villagers discovered based on ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save | Villager")
	TArray<int32> VillagersDiscovered;
	//Contains list of difficulties won
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save | Difficulty")
	TArray<EDifficulty> DifficultiesWon;
	//Key is difficulty, value is high score
	//Difficulties based on 0, 1, 2 (Easy, Normal, Hard)
	//Keeps track of wave high scores in each difficulty respectively
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save | Wave")
	TMap<EDifficulty, int32> DifficultyWaveHighScore = {
		{EDifficulty::Easy, 1},
		{EDifficulty::Normal, 1},
		{EDifficulty::Hard, 1}
	};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save | Wave")
	int32 WaveHighestScore;

};
