// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#include "UDDFileBlueprintLibrary.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"

void UUDDFileBlueprintLibrary::WriteToFile(FString Content, FString FileName, FString Directory, bool AllowOverwriting)
{
	uint32 WriteFlags = 0;
	if (!AllowOverwriting) {
		WriteFlags = FILEWRITE_Append;
	}

	FFileHelper::EEncodingOptions EncodingOptions = FFileHelper::EEncodingOptions::AutoDetect;
	IFileManager* FileManager = &IFileManager::Get();

	if (FFileHelper::SaveStringToFile(Content, *FPaths::Combine(Directory, FileName), EncodingOptions, FileManager, WriteFlags)) {
		UE_LOG(LogTemp, Log, TEXT("Successfully wrote to file %s"), *FileName);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not write to file %s - Check if the path is valid, or if the file is read only."), *FileName);
	}
}

FString UUDDFileBlueprintLibrary::ReadFromFile(FString FileName, FString Directory)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*FPaths::Combine(Directory, FileName))) {
		FString Content;
		FFileHelper::LoadFileToString(Content, *FPaths::Combine(Directory, FileName));
		return Content;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not read from file %s - Check if the path is valid, or if the file exists."), *FileName);
	}

	return FString();
}
