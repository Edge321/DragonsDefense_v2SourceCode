// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableStateInterface.h"

void IDDPlaceableStateInterface::PrintWrongActorError(const AActor* Actor) const
{
	UE_LOG(LogTemp, Fatal, TEXT("%s is not a valid actor to pass in"), *Actor->GetName())
}

void IDDPlaceableStateInterface::PrintNullPtrError() const
{
	UE_LOG(LogTemp, Fatal, TEXT("Actor passed in was a nullptr!"))
}
