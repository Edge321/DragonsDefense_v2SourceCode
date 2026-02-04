// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
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
	UPROPERTY(VisibleAnywhere, Category = "Save | Villager")
	TArray<int32> VillagersDiscovered;
	//Difficulties won based on 0, 1, 2 (Easy, Normal, Hard)
	UPROPERTY(VisibleAnywhere, Category = "Save | Difficulty")
	TArray<int32> DifficultiesWon;
	//Key is difficulty, value is high score
	//Difficulties based on 0, 1, 2 (Easy, Normal, Hard)
	//Keeps track of wave high scores in each difficulty respectively
	UPROPERTY(VisibleAnywhere, Category = "Save | Wave")
	TMap<int32, int32> DifficultyWaveHighScore = {
		{0, 1},
		{1, 1},
		{2, 1}
	};
	UPROPERTY(VisibleAnywhere, Category = "Save | Wave")
	int32 WaveHighestScore;

};
