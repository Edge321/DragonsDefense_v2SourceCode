// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDSpikeTrap.h"

void ADDSpikeTrap::EnableDebuffArea()
{
	if (DebuffAreaRef.IsValid()) {
		DebuffAreaRef->EnableDebuffArea();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s does not have a valid pointer to DebuffArea"), *GetName())
	}
}

void ADDSpikeTrap::DisableDebuffArea()
{
	if (DebuffAreaRef.IsValid()) {
		DebuffAreaRef->DisableDebuffArea();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s does not have a valid pointer to DebuffArea"), *GetName())
	}
}
