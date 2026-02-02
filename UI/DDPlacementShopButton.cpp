// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlacementShopButton.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDPlaceableManager.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"

void UDDPlacementShopButton::InitializeButton()
{
	Super::InitializeButton();
	UImage* ButtonImage = nullptr;
	TArray<UWidget*> Widgets = GetAllChildren();

	for (UWidget* Widget : Widgets) {
		if (Widget && Widget->IsA<UImage>()) {
			ButtonImage = Cast<UImage>(Widget);
			break;
		}
	}

	if (!ButtonImage) {
		UE_LOG(LogTemp, Warning, TEXT("%s does not have an image child"), *GetName());
		return;
	}

	UTexture2D* PlaceableImage = PlaceableClass.GetDefaultObject()->PlaceableData->PlaceableImage;
	if (!PlaceableImage) {
		UE_LOG(LogTemp, Warning, TEXT("%s does not have an image"), *GetName());
		return;
	}

	ButtonImage->SetBrushFromTexture(PlaceableImage);
}

void UDDPlacementShopButton::PlacementMode(bool IsPlacing) const
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameMode) {
		GameMode->IsPlacing(IsPlacing);
	}
}

void UDDPlacementShopButton::OnClickEventFunction()
{
	if (GetBuyableStatus()) {
		PlacementMode(true);
		SetPreviewAndPrice();
		OnClickPlacementButton.Broadcast();
	}
}

void UDDPlacementShopButton::OnHoveredEventFunction()
{
	OnPlaceableHovered.Broadcast(this, PlaceableClass);
}

void UDDPlacementShopButton::SetPreviewAndPrice() const
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameMode) {
		ADDPlaceableManager& Manager = GameMode->GetPlaceableManager();
		Manager.SetCurrentPlaceable(PlaceableClass, Price);
	}
}
