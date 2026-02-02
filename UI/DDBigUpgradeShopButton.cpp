// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDBigUpgradeShopButton.h"
#include "Components/Image.h"

void UDDBigUpgradeShopButton::OnClickEventFunction()
{
	EnableGreenBorder();
	OnBigShopButtonClicked.Broadcast(this, nullptr);
}

void UDDBigUpgradeShopButton::InitializeButton()
{
	Super::InitializeButton();
	if (ButtonIcon) {
		TArray<UWidget*> Children = GetAllChildren();
		for (UWidget* Child : Children) {
			if (Child->IsA<UImage>()) {
				UImage* Image = Cast<UImage>(Child);
				Image->SetBrushFromTexture(ButtonIcon);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to set ButtonIcon for %s"), *GetName())
	}
}

bool UDDBigUpgradeShopButton::TryBuy()
{
	//As if it was clicked
	Super::OnClickEventFunction();

	return true;
}
