// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/DDDowngradeDataStruct.h"
#include "DragonsDefense_v2/Game/DDDowngradeTypeInterface.h"
#include "DDDowngradeStruct.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FDDDowngradeStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Description")
	FString Name;
	UPROPERTY(EditAnywhere, Category = "Description")
	FString Description;
	UPROPERTY(EditAnywhere, Category = "Description")
	FString RewardDescription;

	UPROPERTY(EditAnywhere)
	TArray<FDDDowngradeDataStruct> DowngradeDataArray;
	//How many waves downgrade last
	UPROPERTY(EditAnywhere)
	int32 DowngradeDuration = 1;

	UPROPERTY(EditAnywhere)
	TArray<FDDDowngradeDataStruct> RewardDataArray;
	//How many waves reward last
	UPROPERTY(EditAnywhere)
	int32 RewardDuration = 1;
	
	//Requirement for being able to get this downgrade at a certain wave number. Default is first wave.
	UPROPERTY(EditAnywhere)
	int32 UnlockedWave = 1;

	//If this downgrade is special and should be reserved for slot 4 of the Negative Soul Shop (Typically mini-boss)
	UPROPERTY(EditAnywhere)
	bool bIsSpecialCard = false;
	//Reserved for the final boss of the game, which will officially end the game after beating the wave
	UPROPERTY(EditAnywhere)
	bool bIsFinalCard = false;

};