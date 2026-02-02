// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UDDFileBlueprintLibrary.generated.h"

/**
 * Library of functions for file manipulation, since blueprints does not have standard
 * functions for this sort of thing
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UUDDFileBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	/// <summary>
	/// Writes a string to a file
	/// </summary>
	/// <param name="Content">What string to write to the file</param>
	/// <param name="FileName">The name of the file that will be written to</param>
	/// <param name="Directory">The path to the file that is being written to</param>
	/// <param name="AllowOverwriting">Enable appending content to the existing file</param>
	UFUNCTION(BlueprintCallable, Category = "File")
	static void WriteToFile(FString Content, FString FileName, FString Directory, bool AllowOverwriting);

	/// <summary>
	/// Reads contents from an existing file
	/// </summary>
	/// <param name="FileName">The name of the file being read</param>
	/// <param name="Directory">The path of the file being read</param>
	/// <returns>Contents of the existing file</returns>
	UFUNCTION(BlueprintCallable, Category = "File")
	static FString ReadFromFile(FString FileName, FString Directory);

};
