// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDBigPlacementShopButton.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"

void UDDBigPlacementShopButton::OnClickEventFunction()
{
	EnableGreenBorder();
	InitializeDescription();
	OnBigShopButtonClicked.Broadcast(this, PlaceableClass);
}

bool UDDBigPlacementShopButton::TryBuy()
{
	//As if it was clicked
	Super::OnClickEventFunction();

	return true;
}

void UDDBigPlacementShopButton::InitializeDescription()
{
	if (bHasDescriptionInitialized) return;

	UDDPlaceableDataAsset* PlaceableData = PlaceableClass.GetDefaultObject()->PlaceableData;

	ButtonIcon = PlaceableData->PlaceableImage;

	bHasDescriptionInitialized = true;
}
