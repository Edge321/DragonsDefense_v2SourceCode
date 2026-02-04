// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDMapBlueprintLibrary.h"
#include "Engine/DataTable.h"
//My classes
#include "DragonsDefense_v2/Characters/DDPlaceable.h"

TArray<FDDPlaceableAmountPair> UDDMapBlueprintLibrary::ConvertMapToArrayPair(TMap<TSubclassOf<ADDPlaceable>, int32> PlaceablesMap)
{
	TArray<FDDPlaceableAmountPair> PlaceableArray;

	for (TPair<TSubclassOf<ADDPlaceable>, int32> PlaceablePair : PlaceablesMap) {
		FDDPlaceableAmountPair Pair{
			PlaceablePair.Key,
			PlaceablePair.Value
		};

		PlaceableArray.Add(Pair);
	}

	return PlaceableArray;
}

TArray<int32> UDDMapBlueprintLibrary::ConvertIntMapToArray(TMap<int32, int32> IntMap) 
{
	TArray<int32> IntArray;

	for (TPair<int32, int32> IntPair : IntMap) {
		IntArray.Add(IntPair.Value);
	}

	return IntArray;
}

TMap<FString, FString> UDDMapBlueprintLibrary::ConvertDataTableToMap(const UDataTable* DataTable, const FString Seperator)
{
	TMap<FString, FString> DataTableMap;
	FString TempString;
	FString SeperatorString;

	//Aw hell nah, O(n^2) (theoritically)
	TArray<FName> RowNames = DataTable->GetRowNames();
	for (FName Name : RowNames) {
		bool FoundSeperator = false;
		TempString = Name.ToString();
		SeperatorString.Empty();

		for (int32 i = 0; i < TempString.Len(); i++) {
			char C = TempString[i];
			if (FoundSeperator) {
				SeperatorString.AppendChar(C);
				continue;
			}
			
			FoundSeperator = (C == Seperator[0]);
		}

		if (FoundSeperator) {
			DataTableMap.Add(SeperatorString, TempString);
		}
	}

	return DataTableMap;
}
