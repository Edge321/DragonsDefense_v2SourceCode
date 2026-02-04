// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DDMapBlueprintLibrary.generated.h"

class ADDPlaceable;
class UDDUpgradeShopButton;
class UDataTable;
enum class EPlayerStats : uint8;

USTRUCT(BlueprintType)
struct FDDPlaceableAmountPair {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ADDPlaceable> PlaceableClass;
	UPROPERTY(BlueprintReadOnly)
	int32 PlaceableAmount;
};

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDMapBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Conversion")
	static TArray<FDDPlaceableAmountPair> ConvertMapToArrayPair(TMap<TSubclassOf<ADDPlaceable>, int32> PlaceablesMap);
	//Used to convert difficulty map from save file to array
	UFUNCTION(BlueprintCallable, Category = "Conversion")
	static TArray<int32> ConvertIntMapToArray(TMap<int32, int32> IntMap);

	/// <summary>
	/// Uses the rows of Data Tables, and converts it to a map, based on the Seperator variable.
	/// The value is the row name, and the Key is the string after the Seperator
	/// </summary>
	/// <param name="DataTable"></param>
	/// <param name="Seperator">Character that defines key and value. Should only be one character</param>
	/// <returns></returns>
	UFUNCTION(BlueprintCallable, Category = "Conversion")
	static TMap<FString, FString> ConvertDataTableToMap(const UDataTable* DataTable, const FString Seperator = ".");

};
