// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDActorLibrary.h"

AActor* UDDActorLibrary::SpawnDeferredActorFromClass(UObject* WorldContextObject, UClass* Class, const FTransform &Transform) 
{
	if (!WorldContextObject) {
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) {
		return nullptr;
	}

	return World->SpawnActorDeferred<AActor>(Class, Transform);
}
