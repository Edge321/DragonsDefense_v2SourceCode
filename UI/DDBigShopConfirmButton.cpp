// Copyright (c) 2026, Edge Cope Corp. All rights reserved


#include "DDBigShopConfirmButton.h"
#include "Components/TextBlock.h"
//My classes
#include "DragonsDefense_v2/UI/DDShopButton.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"

void UDDBigShopConfirmButton::InitializeButton() 
{
	OriginalButtonColor = GetBackgroundColor();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnWaveStart.AddDynamic(this, &UDDBigShopConfirmButton::WaveStartEventFunction);
		GameMode->OnGameOver.AddDynamic(this, &UDDBigShopConfirmButton::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &UDDBigShopConfirmButton::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &UDDBigShopConfirmButton::GameOverEventFunction);
	}

	OnClicked.AddDynamic(this, &UDDBigShopConfirmButton::OnClickEventFunction);
}

UDDShopButton* UDDBigShopConfirmButton::GetCurrentBigShopButtonPtr()
{
	if (BigShopButtonPtr.IsValid()) {
		return BigShopButtonPtr.Get();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("BigShopButtonPtr is null, failed to get pointer"))
		return nullptr;
	}
}

void UDDBigShopConfirmButton::UpdateBigConfirmButton() 
{
	if (BigShopButtonPtr.IsValid()) {
		SetBackgroundColor(BigShopButtonPtr->GetBuyableStatus() ? OriginalButtonColor : UnbuyableColor);
		SetIsEnabled(BigShopButtonPtr->GetBuyableStatus());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("BigShopButtonPtr is null, skipping updating big confirm button"))
	}
}

void UDDBigShopConfirmButton::OnBigShopButtonClicked(UDDShopButton* Button)
{
	if (BigShopButtonPtr.Get() == Button) return; //do nothing

	if (BigShopButtonPtr.IsValid()) {
		BigShopButtonPtr->DeselectButton();
	}

	if (Button) {
		BigShopButtonPtr = MakeWeakObjectPtr(Button);
		ButtonText->SetText(FText::FromString(BigShopButtonPtr->GetButtonBuyText()));
		UpdateBigConfirmButton();
	}
}

void UDDBigShopConfirmButton::OnClickEventFunction()
{
	if (BigShopButtonPtr.IsValid()) {
		if (BigShopButtonPtr->GetBuyableStatus()) {
			BigShopButtonPtr->TryBuy();
			OnBuy.Broadcast();
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("BigShopButtonPtr is invalid. How is this button reachable in the first place?"))
	}
}

void UDDBigShopConfirmButton::WaveStartEventFunction()
{
	if (BigShopButtonPtr.IsValid()) {
		BigShopButtonPtr->DeselectButton();
		BigShopButtonPtr.Reset();
	}
}

void UDDBigShopConfirmButton::GameOverEventFunction() 
{
	if (BigShopButtonPtr.IsValid()) {
		BigShopButtonPtr->DeselectButton();
		BigShopButtonPtr.Reset();
	}
}