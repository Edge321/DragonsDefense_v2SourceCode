// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDNegativeSoulShopWidget.h"
//My classes
#include "DragonsDefense_v2/Game/DDNegativeSoulShopManager.h"
#include "DragonsDefense_v2/Game/DDDowngrade.h"

void UDDNegativeSoulShopWidget::ValidateManager()
{
	check(Manager);
}

void UDDNegativeSoulShopWidget::RelayDowngrade(UDDDowngrade* Downgrade)
{
	Manager->ReceiveDowngrade(Downgrade);
	CurrentActiveDowngrade = Downgrade;
}

void UDDNegativeSoulShopWidget::BroadcastNegativeShopClose() const
{
	OnCloseNegativeShop.Broadcast();
}

const FString UDDNegativeSoulShopWidget::GetDowngradeName(const UDDDowngrade* Downgrade) const
{
	return Downgrade->GetDowngradeName();
}

const FString UDDNegativeSoulShopWidget::GetDowngradeDescription(const UDDDowngrade* Downgrade) const
{
	return Downgrade->GetDescription();
}

const FString UDDNegativeSoulShopWidget::GetDowngradeRewardDescription(const UDDDowngrade* Downgrade) const
{
	return Downgrade->GetRewardDescription();
}

void UDDNegativeSoulShopWidget::SetDowngradeSelection(const int32 DowngradeSelected)
{
	int32 LowerIndex = 0;
	int32 UpperIndex = 2;
	
	if (DowngradeSelected >= LowerIndex || DowngradeSelected <= UpperIndex) {
		CurrentSelection = DowngradeSelected;
	}
}

const int32 UDDNegativeSoulShopWidget::GetDowngradeSelection() const
{
	return CurrentSelection;
}

void UDDNegativeSoulShopWidget::ResetDowngradeSelection()
{
	CurrentSelection = -1;
}

void UDDNegativeSoulShopWidget::SetManager(ADDNegativeSoulShopManager* NewManager)
{
	Manager = NewManager;
}

void UDDNegativeSoulShopWidget::SetDowngrades(TArray<UDDDowngrade*> Downgrades)
{
	CurrentDowngrades = Downgrades;
	DisplayDowngrades();
}

void UDDNegativeSoulShopWidget::SetIsDowngradeActive(const bool Active)
{
	bIsDowngradeActive = Active;
}

void UDDNegativeSoulShopWidget::SetIsRewardActive(const bool Active)
{
	bIsRewardActive = Active;
}

void UDDNegativeSoulShopWidget::RequestDowngrade()
{
	if (CurrentSelection >= 0) {
		Manager->ReceiveReward(CurrentDowngrades[CurrentSelection]);
	}
	else {
		Manager->ReceiveReward(nullptr);
	}
}
