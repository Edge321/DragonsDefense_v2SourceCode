// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DragonsDefense_v2/Game/DDDowngradeStruct.h"
#include "DDDowngrade.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class DRAGONSDEFENSE_V2_API UDDDowngrade : public UObject
{
	GENERATED_BODY()

public:

	void Initalize(FDDDowngradeStruct StructDowngrade);

	const FString GetDowngradeName() const;
	const FString GetDescription() const;
	const FString GetRewardDescription() const;
	void InitDowngradeReward(UObject* Outer);
	void InitDataArrays(TArray<FDDDowngradeDataStruct>& DataArray, UObject* Outer);
	const int32 GetDowngradeDuration() const;
	const int32 GetRewardDuration() const;
	const int32 GetUnlockedWave() const;
	//Used for only the final boss card
	void SetUnlockedWave(const int32 Wave);
	const bool GetIsFinalCard() const;
	const TArray<FDDDowngradeDataStruct> GetDowngradeDataArray() const;
	const TArray<FDDDowngradeDataStruct> GetRewardDataArray() const;

	UFUNCTION(BlueprintPure)
	const bool GetIsSpecialCard() const;

private:

	void ValidateClassVariables() const;

	UPROPERTY()
	struct FDDDowngradeStruct DowngradeData;
};
