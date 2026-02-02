// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDebuffBase.h"

void UDDDebuffBase::PrintWrongActorError() const
{
	UE_LOG(LogTemp, Error, TEXT("Wrong actor passed in"))
}

void UDDDebuffBase::SetDebuffAmount(const float NewDebuffAmount)
{
	DebuffAmount = NewDebuffAmount;
}
