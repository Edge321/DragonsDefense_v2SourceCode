// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDShopButton.h"
#include "Components/Overlay.h"
#include "Components/Border.h"
#include "Components/Image.h"
//My classes
#include "DragonsDefense_v2/Lib/DDWidgetBlueprintLibrary.h"

void UDDShopButton::ValidatePriceText()
{
	if (PriceText == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Error: %s does not have a price text reference"), *GetName())
	}
}

void UDDShopButton::InitializeButton()
{	
	UpdateText();
	SetBackgroundColor(UnBuyableColor);
	OnClicked.AddDynamic(this, &UDDShopButton::OnClickEventFunction);
	OnHovered.AddDynamic(this, &UDDShopButton::OnHoveredEventFunction);
	
	ActualPriceString = PriceText->GetText().ToString();

	if (ItemNameText) {
		ActualItemNameString = ItemNameText->GetText().ToString();
	}

	UBorder* BorderParent = UDDWidgetBlueprintLibrary::SearchWidgetParent<UBorder>(this, BorderSearchAmount);
	if (BorderParent) {
		GreenBorderOutline = BorderParent;
		GreenBorderOutline->SetBrushColor(BorderColor);
		DisableGreenBorder();
	}

	UOverlay* OverlayParent = UDDWidgetBlueprintLibrary::SearchWidgetParent<UOverlay>(this, OverlaySearchAmount);
	if (OverlayParent) {
		for (UWidget* Child : OverlayParent->GetAllChildren()) {
			if (Child->IsA<UDDLockButton>()) {
				LockImageWidget = Cast<UDDLockButton>(Child);
				LockImageWidget->InitializeButton();
				LockImageWidget->OnHovered.AddDynamic(this, &UDDShopButton::OnLockHoveredEventFunction);
				LockImageWidget->OnUnhovered.AddDynamic(this, &UDDShopButton::OnUnhoveredEventFunction);
			}
		}
	}

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

	if (WaveUnlocked <= 1) {
		InstantUnlock();
	}
	else {
		Lock();
	}
}

const FString UDDShopButton::GetDescription() const
{
	return ButtonDescription;
}

void UDDShopButton::OnLockHoveredEventFunction()
{
	if (!LockImageWidget->GetIsLocked()) {
		//TODO - add unlock animation here
		InstantUnlock();
	}
	else {
		OnLockButtonHovered.Broadcast(this);
	}
}

void UDDShopButton::EnableGreenBorder()
{
	GreenBorderOutline->SetPadding(BorderEnabled);
	//Make border visible
	FLinearColor Outline = GreenBorderOutline->GetBrushColor();
	Outline.A = 255.0f;
	GreenBorderOutline->SetBrushColor(Outline);
}

void UDDShopButton::DisableGreenBorder()
{
	GreenBorderOutline->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f));
	//Make border invisible
	FLinearColor Outline = GreenBorderOutline->GetBrushColor();
	Outline.A = 0.0f;
	GreenBorderOutline->SetBrushColor(Outline);
}

void UDDShopButton::OnUnhoveredEventFunction()
{
	OnUnhovered.Broadcast();
}

void UDDShopButton::InstantUnlock()
{
	if (LockImageWidget) {
		LockImageWidget->SetVisibility(ESlateVisibility::Hidden);
		LockImageWidget->SetIsLocked(false);
	}
	SetIsEnabled(true);

	if (ItemNameText) {
		ItemNameText->SetText(FText::FromString(ActualItemNameString));
	}
	PriceText->SetText(FText::FromString(ActualPriceString));
}

const bool UDDShopButton::GetBuyableStatus()
{
	IsBuyable();
	return bIsBuyable;
}

const UTexture2D* UDDShopButton::GetButtonIcon() const
{
	return ButtonIcon;
}

const FString UDDShopButton::GetButtonBuyText() const
{
	return ButtonBuyText;
}

const bool UDDShopButton::GetTooltipEnabled() const
{
	return bTooltipEnabled;
}

const int32 UDDShopButton::GetWaveUnlocked() const
{
	return WaveUnlocked;
}

void UDDShopButton::GameOverEventFunction()
{
	bIsBuyable = false;
	SetBackgroundColor(UnBuyableColor);
}

void UDDShopButton::Unlock()
{
	if (LockImageWidget) {
		LockImageWidget->SetIcon(UnlockedIcon);
		LockImageWidget->SetIsLocked(false);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s does not have a LockImageWidget set"), *GetName())
	}

	IsBuyable();
}

void UDDShopButton::Lock() 
{
	if (LockImageWidget) {
		LockImageWidget->SetIcon(LockedIcon);
		LockImageWidget->SetVisibility(ESlateVisibility::Visible);
		LockImageWidget->SetIsLocked(true);
	}

	if (ItemNameText) {
		ItemNameText->SetText(FText::FromString("?"));
	}
	PriceText->SetText(FText::FromString("?"));
	SetIsEnabled(false);
}

void UDDShopButton::DeselectButton()
{
	DisableGreenBorder();
}
