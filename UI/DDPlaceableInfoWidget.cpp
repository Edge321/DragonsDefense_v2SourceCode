// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableInfoWidget.h"
//My classes
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"
#include "DragonsDefense_v2/Characters/DDTrapPlaceable.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDPlaceableManager.h"
#include "DragonsDefense_v2/UI/DDSoulShopWidget.h"

void UDDPlaceableInfoWidget::InitializeInfoWidget()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GetSoulShopWidget().OnRequestShowAllPlaceableInfo.BindUObject(this, &UDDPlaceableInfoWidget::ShowAllPlaceableInfo);
		GameMode->GetSoulShopWidget().OnRequestHideAllPlaceableInfo.BindUObject(this, &UDDPlaceableInfoWidget::HideAllPlaceableInfo);
		PlaceableManager = &(GameMode->GetPlaceableManager());
	}
}

void UDDPlaceableInfoWidget::SetCurrentPlaceable(ADDPlaceable* CurrentPlaceable)
{
	//If clicked on placeable is the same as currently stored one
	if (CurrentPlaceable == MostRecentPlaceable) return;

	MostRecentPlaceable = CurrentPlaceable;
	
	//Resetting variables if nullptr
	if (!CurrentPlaceable) {
		Sentient = nullptr;
		Trap = nullptr;
		MostRecentPlaceable = nullptr;
		return;
	}

	if (CurrentPlaceable->IsA<ADDSentientPlaceable>()) {
		bIsSentient = true;
		Sentient = Cast<ADDSentientPlaceable>(CurrentPlaceable);
	}
	else {
		bIsSentient = false;
		Trap = Cast<ADDTrapPlaceable>(CurrentPlaceable);
	}
}

void UDDPlaceableInfoWidget::SetCurrentHoveredPlaceable(ADDPlaceable* CurrentPlaceable)
{
	if (CurrentPlaceable == MostRecentHoveredPlaceable) return;

	MostRecentHoveredPlaceable = CurrentPlaceable;
}

void UDDPlaceableInfoWidget::ChangeBehavior(const EPlaceableAI AI)
{
	if (bIsSentient) {
		Sentient->SetAI(AI);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Attempted to modify TrapPlaceable"))
	}
}

FVector2D UDDPlaceableInfoWidget::CheckOutOfBounds(const FVector2D PanelSize, const FVector2D CursorLocation) const
{
	FVector2D DesiredPosition = CursorLocation;
	//Really make sure widget is adjusted correctly
	float Insurance = 50.0f;
	//Half since position of cursor in the middle of the panel
	FVector2D HalfPanel = PanelSize / 2;
	FVector2D ScreenSize;

	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ScreenSize);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Something went wrong with GEngine!"))
	}

	//Adjust on X-axis
	if (CursorLocation.X + HalfPanel.X > ScreenSize.X) {
		DesiredPosition.X = ScreenSize.X - (HalfPanel.X + Insurance);
	}
	else if (CursorLocation.X - HalfPanel.X < 0) {
		DesiredPosition.X = HalfPanel.X + Insurance;
	}
	//Adjust on Y-axis
	if (CursorLocation.Y + HalfPanel.Y > ScreenSize.Y) {
		DesiredPosition.Y = ScreenSize.Y - (HalfPanel.Y + Insurance);
	}
	else if (CursorLocation.Y - HalfPanel.Y < 0) {
		DesiredPosition.Y = HalfPanel.Y + Insurance;
	}

	return DesiredPosition;
}

void UDDPlaceableInfoWidget::GetStats()
{
	float Damage;

	if (bIsSentient) {
		Damage = MostRecentPlaceable->GetDamage();
		//TODO - Address this later tbh
	}
	else {

	}
}

void UDDPlaceableInfoWidget::ShowAllPlaceableInfo()
{
	if (PlaceableManager.IsValid()) {
		PlaceableManager->ShowAllPlaceablesInfo();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Placeable Manager was not assigned!"))
	}
}

void UDDPlaceableInfoWidget::HideAllPlaceableInfo() 
{
	if (PlaceableManager.IsValid()) {
		PlaceableManager->HideAllPlaceablesInfo();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Placeable Manager was not assigned!"))
	}
}

