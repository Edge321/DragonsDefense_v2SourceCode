// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDSentientStateInterface.h"

void UDDSentientStateInterface::SetOwner(AActor* Actor)
{
	if (Actor->IsA<ADDSentientPlaceable>()) {
		ADDSentientPlaceable* Sentient = Cast<ADDSentientPlaceable>(Actor);		
		SentientOwner = MakeWeakObjectPtr(Sentient);
	}
	else {
		PrintWrongActorError(Actor);
	}
}
