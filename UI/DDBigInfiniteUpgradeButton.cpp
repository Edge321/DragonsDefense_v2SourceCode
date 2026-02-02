// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDBigInfiniteUpgradeButton.h"

void UDDBigInfiniteUpgradeButton::OnClickEventFunction()
{
	EnableGreenBorder();
	OnBigShopButtonClicked.Broadcast(this, nullptr);
}

bool UDDBigInfiniteUpgradeButton::TryBuy()
{
	//As if it was clicked
	Super::OnClickEventFunction();

	return true;
}