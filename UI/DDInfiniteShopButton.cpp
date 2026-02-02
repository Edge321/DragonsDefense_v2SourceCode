// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDInfiniteShopButton.h"

void UDDInfiniteShopButton::UpdateSouls()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->UpdateSouls(-Price);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("How did we get here?"))
	}
}

void UDDInfiniteShopButton::UpdateText()
{
	PriceText->SetText(FetchFTextPrice());
}

void UDDInfiniteShopButton::IsBuyable()
{
	if (LockImageWidget && LockImageWidget->GetIsLocked()) return;

	int32 Souls;

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

	if (Souls >= Price && bIsAuthorized) {
		bIsBuyable = true;
		SetBackgroundColor(BuyableColor);
	}
	else {
		bIsBuyable = false;
		SetBackgroundColor(UnBuyableColor);
	}
}

void UDDInfiniteShopButton::EnableButton()
{
	bIsAuthorized = true;
	IsBuyable();
}

const int32 UDDInfiniteShopButton::GetCurrentPrice() const
{
	return Price;
}

void UDDInfiniteShopButton::DisableButton()
{
	SetBackgroundColor(UnBuyableColor);
	bIsAuthorized = false;
}

const FText UDDInfiniteShopButton::FetchFTextPrice() const
{
	FString PriceString = FString::FromInt(Price);
	return FText::FromString(PriceString);
}
