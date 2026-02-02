// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#include "DDUpgradeShopButton.h"

void UDDUpgradeShopButton::OnClickEventFunction()
{
	if (GetBuyableStatus()) {
		Upgrade(StatUpgrade);
		IncreasePrice();
		UpdateSouls();
		OnUpgrade.Broadcast(this);
	}
}

void UDDUpgradeShopButton::OnHoveredEventFunction()
{
	OnUpgradeHovered.Broadcast(this);
}

void UDDUpgradeShopButton::InitializeButton()
{
	Super::InitializeButton();

	PrefixSumPrices.Init(0, Prices.Num());
	PrefixThreatLevels.Init(0, Prices.Num());

	int32 TempSum = 0;
	int32 TempThreatLevel = 0;
	for (int32 i = 0; i < Prices.Num(); i++) {
		TempSum += Prices[i];
		TempThreatLevel += ThreatLevel;
		PrefixSumPrices[i] = TempSum;
		PrefixThreatLevels[i] = TempThreatLevel;
	}
}

const EPlayerStats UDDUpgradeShopButton::GetStatUpgrade() const
{
	return StatUpgrade;
}

const FDDSecretStats UDDUpgradeShopButton::GetSecretStats() const
{
	return SecretStats;
}

const float UDDUpgradeShopButton::GetThreatLevel() const
{
	return ThreatLevel * PriceIndex;
}

const float UDDUpgradeShopButton::GetPrefixThreatLevel(int32 Index) const
{
	return PrefixThreatLevels[Index];
}

const int32 UDDUpgradeShopButton::GetPriceIndex() const
{
	return PriceIndex;
}

const int32 UDDUpgradeShopButton::GetPrefixSumPrice(int32 Index) const
{
	return PrefixSumPrices[Index];
}

void UDDUpgradeShopButton::UpdateText()
{
	PriceText->SetText(FetchFTextPrice());
}

bool UDDUpgradeShopButton::IsMaxxedOut()
{
	bool MaxxedOut = false;
	if (PriceIndex + 1 > Prices.Num()) {
		PriceText->SetText(FText::FromString("MAX"));
		//SetIsEnabled(false);
		MaxxedOut = true;
		SetBackgroundColor(UnBuyableColor);
	}

	return MaxxedOut;
}

void UDDUpgradeShopButton::IsBuyable()
{
	if (LockImageWidget && LockImageWidget->GetIsLocked()) return;

	int32 Souls;

	if (IsMaxxedOut()) {
		bIsBuyable = false;
		return;
	}

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		Souls = GameMode->GetSouls();
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Something went wrong with fetching GameMode!"))
		bIsBuyable = false;
		SetBackgroundColor(UnBuyableColor);
		return;
	}

	if (Souls >= Prices[PriceIndex]) {
		bIsBuyable = true;
		SetBackgroundColor(BuyableColor);
	}
	else {
		bIsBuyable = false;
		SetBackgroundColor(UnBuyableColor);
	}

}

void UDDUpgradeShopButton::Upgrade(EPlayerStats Stat) const
{
	ADDPlayer* Player = nullptr;
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		//Doing some mumbo jumbo here as I want Player to be outside the if-statement scope
		Player = &(GameMode->GetPlayer());
	}

	check(Player)

	switch (Stat) {
		case EPlayerStats::MovementSpeed:
			Player->UpdateMovementSpeed(UpgradeAmount);
			break;
		case EPlayerStats::ShootSpeed:
			Player->UpdateShootSpeed(UpgradeAmount);
			break;
		case EPlayerStats::MaxHealth:
			Player->UpdateMaxHealth(UpgradeAmount);
			break;
		case EPlayerStats::Health:
			Player->UpdateHealth(UpgradeAmount);
			break;
		case EPlayerStats::Damage:
			Player->UpdateDamage(UpgradeAmount);
			break;
		case EPlayerStats::ProjectileSpeed:
			Player->UpdateProjectileSpeed(UpgradeAmount);
			break;
		case EPlayerStats::Penetration:
			Player->UpdatePenetration(UpgradeAmount);
			break;
		case EPlayerStats::CriticalChance:
			Player->UpdateCritProjChance(UpgradeAmount);
			break;
		case EPlayerStats::ProjectileSize:
			Player->UpdateProjectileSize(UpgradeAmount);
			break;
		case EPlayerStats::LineOfSight:
			Player->UpdateLineOfSightLength(UpgradeAmount);
		case EPlayerStats::Foresight:
			//Not implemented yet
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Error: No upgrade set to %s. Bad things will occur"), *GetName())
			break;
	}
}

const int32 UDDUpgradeShopButton::GetPricesSize() const
{
	return Prices.Num();
}

const FString UDDUpgradeShopButton::GetPlayerStatToString() const
{
	return PlayerStatStrings[(uint8)StatUpgrade];
}

const FString UDDUpgradeShopButton::GetPlayerStatGrade() const
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		return GameMode->GetPlayer().GetPlayerStatFuzzification(StatUpgrade);
	}

	return "N/A";
}

void UDDUpgradeShopButton::IncreasePrice()
{
	PriceIndex++;
	if (!IsMaxxedOut()) {
		UpdateText();
	}
}

const int32 UDDUpgradeShopButton::GetCurrentPrice() const
{
	//TODO - fix overflow from Big Soul Shop Confirm Button, when maxxing out an upgrade
	return Prices[PriceIndex];
}

void UDDUpgradeShopButton::UpdateSouls()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		/*IncreasePrice();*/
		GameMode->UpdateSouls(-Prices[PriceIndex - 1]);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("How did we get here?"))
	}
}

void UDDUpgradeShopButton::GameOverEventFunction()
{
	Super::GameOverEventFunction();
	ResetPrice();
	UpdateText();
	//SetIsEnabled(true);
}

void UDDUpgradeShopButton::ResetPrice()
{
	PriceIndex = 0;
	PriceText->SetText(FetchFTextPrice());
}

const FText UDDUpgradeShopButton::FetchFTextPrice() const
{
	if (PriceIndex >= Prices.Num()) {
		return FText::FromString("MAX");
	}
	
	FString PriceString = FString::FromInt(Prices[PriceIndex]);
	return FText::FromString(PriceString);
}