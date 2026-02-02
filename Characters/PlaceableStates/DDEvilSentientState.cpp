// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEvilSentientState.h"
//My classes
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"

void UDDEvilSentientState::OnInitialize(AActor* Owner)
{
	PlaceablesInArea.Empty();

	if (Owner) {
		SetOwner(Owner);
	}
	else {
		PrintNullPtrError();
	}
	SentientOwner->SetAttackFunction(std::bind(&UDDEvilSentientState::BeginAttack, this));
	SellingPriceBeforeEvil = SentientOwner->GetSellingPrice();
	//Evil is worthless
	SentientOwner->SetSellingPrice(0);
	//x1.5 attack radius for being evil
	AttackRadiusBeforeEvil = SentientOwner->GetAttackRadius();
	SentientOwner->UpdateAttackRadius(AttackRadiusBeforeEvil * EvilAttackRadiusMultiplier);
	SentientOwner->OnTurnEvil.ExecuteIfBound(SentientOwner.Get());

	StoreAnyPlaceables(SentientOwner->GetAllActorsInRadius());
}

void UDDEvilSentientState::HandleBeginOverlap(AActor* Actor)
{
	if (Actor && Actor->IsA<ADDPlaceable>()) {
		ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
		if (!Placeable->IsEvil()) {
			PlaceablesInArea.Add(Placeable);
			SentientOwner->StartAttack();
		}
	}
}

void UDDEvilSentientState::HandleEndOverlap(AActor* Actor)
{
	if (Actor && Actor->IsA<ADDPlaceable>()) {
		ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
		PlaceablesInArea.RemoveSwap(Placeable);
		if (PlaceablesInArea.IsEmpty()) {
			SentientOwner->Deactive();
		}
	}
}

void UDDEvilSentientState::OnUninitialize()
{
	PlaceablesInArea.Empty();
	if (SentientOwner.IsValid()) {
		SentientOwner->UpdateAttackRadius(-(AttackRadiusBeforeEvil * EvilAttackRadiusMultiplier));
		SentientOwner->SetSellingPriceExactly(SellingPriceBeforeEvil);
	}
	SentientOwner.Reset();
}

void UDDEvilSentientState::StoreAnyPlaceables(TArray<AActor*> Actors)
{
	for (AActor* Actor : Actors) {
		if (Actor && Actor->IsA<ADDPlaceable>() && Actor != SentientOwner) {
			ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
			if (!Placeable->IsEvil()) {
				PlaceablesInArea.Add(Placeable);
			}
		}
	}

	if (!PlaceablesInArea.IsEmpty()) {
		SentientOwner->StartAttack();
	}
}

void UDDEvilSentientState::BeginAttack()
{
	if (PlaceablesInArea.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("Placeables array is empty, %s unable to attack"), *SentientOwner->GetName())
		SentientOwner->Deactive();
		return;
	}

	int32 RandomPlaceableIndex = FMath::RandRange(0, PlaceablesInArea.Num() - 1);
	ADDPlaceable* Placeable = PlaceablesInArea[RandomPlaceableIndex];

	//Check evilness since its possible for placeable to be in array and turn evil
	while (!PlaceablesInArea.IsEmpty()) {
		if (Placeable->IsEvil()) {
			PlaceablesInArea.RemoveSwap(Placeable);
			//Check again since just removed element. Prob a better way to go about this but I dont care!!!
			if (!PlaceablesInArea.IsEmpty()) {
				RandomPlaceableIndex = FMath::RandRange(0, PlaceablesInArea.Num() - 1);
				Placeable = PlaceablesInArea[RandomPlaceableIndex];
			}
		}
		else {
			SentientOwner->Attack(Placeable);
			break;
		}
	}
}
