// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPaladinSentient.h"
//My classes
#include "DragonsDefense_v2/Characters/PlaceableStates/DDEvilSentientState.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDPaladinNormalState.h"
#include "DragonsDefense_v2/Characters/DDTinkererEnemy.h"

void ADDPaladinSentient::BeginPlay()
{
	Super::BeginPlay();

	UDDPaladinNormalState* NormalState = NewObject<UDDPaladinNormalState>(this);

	if (NormalState) {
		SetPlaceableState(NormalState);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Something went wrong with allocating Normal paladin state"))
	}
}

void ADDPaladinSentient::ConvertToEvil()
{
	if (!PlaceableState.GetObject()->IsA<UDDEvilSentientState>()) {
		bIsEvil = true;
		UDDEvilSentientState* EvilState = NewObject<UDDEvilSentientState>(this);
		SetPlaceableState(EvilState);
		GetWorldTimerManager().SetTimer(EvilTimer, this, &ADDPaladinSentient::RevertToNormal, TimeToBeEvil);
	}
}

void ADDPaladinSentient::RevertToNormal()
{
	bool bIsInTinkererRadius = false;

	TArray<AActor*> Actors = GetAllActorsInRadius();
	for (AActor* Actor : Actors) {
		if (Actor->IsA<ADDTinkererEnemy>()) {
			bIsInTinkererRadius = true;
			break;
		}
	}

	//Stay evil for longer
	if (bIsInTinkererRadius) {
		GetWorldTimerManager().SetTimer(EvilTimer, this, &ADDPaladinSentient::RevertToNormal, TimeToBeEvil);
	}
	else {
		UDDPaladinNormalState* NormalState = NewObject<UDDPaladinNormalState>(this);
		SetPlaceableState(NormalState);
		bIsEvil = false;
	}
}
