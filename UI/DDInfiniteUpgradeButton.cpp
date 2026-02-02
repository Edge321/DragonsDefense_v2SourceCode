// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDInfiniteUpgradeButton.h"

void UDDInfiniteUpgradeButton::Upgrade(EPlayerStats Stat)
{
	ADDPlayer* Player = nullptr;
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		//Doing some mumbo jumbo here as I want Player to be outside the if-statement scope
		Player = &(GameMode->GetPlayer());
	}

	switch (Stat) {
	case EPlayerStats::Health:
		Player->UpdateHealth(UpgradeAmount);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Error: No upgrade set to %s. Bad things will occur"), *GetName())
			break;
	}
}

void UDDInfiniteUpgradeButton::OnHoveredEventFunction()
{
	OnUpgradeHovered.Broadcast(this);
}

void UDDInfiniteUpgradeButton::OnClickEventFunction()
{
	if (GetBuyableStatus()) {
		Upgrade(UpgradeStat);
		UpdateSouls();
	}
}
