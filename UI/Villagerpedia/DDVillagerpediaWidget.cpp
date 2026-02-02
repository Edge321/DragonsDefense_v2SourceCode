// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDVillagerpediaWidget.h"
//My classes
#include "DragonsDefense_v2/UI/Villagerpedia/VillagerpediaButton.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/UI/DDNewVillagerAlertWidget.h"

void UDDVillagerpediaWidget::InitializeChecklist()
{
	VillagerChecklist.Reserve(VillagerAmount);
	//Filling up map
	for (int32 i = 0; i < VillagerAmount; i++) {
		VillagerChecklist.Add(i, false);
	}
}

void UDDVillagerpediaWidget::BindButtonDelegate(UVillagerpediaButton* Button)
{
	Button->OnSetVillagerPageInfo.BindUObject(this, &UDDVillagerpediaWidget::SetPageInfo);
}

void UDDVillagerpediaWidget::GetNewVillagerAlertReference()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		NewVillagerRef = MakeWeakObjectPtr(&(GameMode->GetNewVillagerAlertWidget()));
	}
}

void UDDVillagerpediaWidget::BindEnemySpawnerDelegate()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GetEnemySpawner().OnSpawnEnemyForID.BindUObject(this, &UDDVillagerpediaWidget::CheckChecklist);
	}
}

void UDDVillagerpediaWidget::SetPreviousWidget(UWidget* NewPreviousWidget)
{
	if (NewPreviousWidget) {
		PreviousWidget = NewPreviousWidget;
		PreviousWidget->SetVisibility(ESlateVisibility::Hidden);
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Invalid widget passed in. Nothing will occur"))
	}
}

void UDDVillagerpediaWidget::BackToPreviousWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
	if (PreviousWidget) {
		PreviousWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("PreviousWidget has not been set. Nothing will occur"))
	}
}

void UDDVillagerpediaWidget::CheckChecklist(const int32 EnemyID, const bool SendAlert)
{
	if (VillagerChecklist.Contains(EnemyID)) {
		if (!VillagerChecklist[EnemyID]) {
			VillagerChecklist[EnemyID] = true;
			UVillagerpediaButton* Button = FetchVillagerpediaButton(EnemyID);
			if (Button) {
				Button->EnableButton();
				Button->EnableNewVillagerFlag();
				if (SendAlert) {
					SendNewVillagerAlert(EnemyID);
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Unable to find VillagerpediaButton with ID %d"), EnemyID);
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("EnemyID %d does not exist in the Villagerpedia"), EnemyID);
	}
}

UVillagerpediaButton* UDDVillagerpediaWidget::FetchVillagerpediaButton(const int32 EnemyID)
{
	for (UVillagerpediaButton* Button : ButtonArray) {
		if (Button) {
			const int32 ID = Button->GetEnemyID();
			if (ID == EnemyID) {
				return Button;
			}
		}
	}
	return nullptr;
}

const TArray<int32> UDDVillagerpediaWidget::GetVillagersDiscovered() const
{
	TArray<int32> VillagersDiscovered;

	for (TPair<int32, bool> VillagerPair : VillagerChecklist) {
		if (VillagerPair.Value) {
			VillagersDiscovered.Add(VillagerPair.Key);
		}
	}

	return VillagersDiscovered;
}

void UDDVillagerpediaWidget::LoadFoundVillagers(TArray<int32> EnemyIDs)
{
	for (int32 EnemyID : EnemyIDs) {
		if (VillagerChecklist.Contains(EnemyID)) {
			if (!VillagerChecklist[EnemyID]) {
				VillagerChecklist[EnemyID] = true;
				UVillagerpediaButton* Button = FetchVillagerpediaButton(EnemyID); //TODO - Very inefficient since this is causing a O(n^2)
				if (Button) {
					Button->EnableButton();
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("Unable to find VillagerpediaButton with ID %d"), EnemyID);
				}
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("EnemyID %d does not exist in the Villagerpedia"), EnemyID);
		}
	}
}

void UDDVillagerpediaWidget::LoadFoundVillagers(int32 EnemyID)
{
	TArray<int32> EnemyIDs;
	EnemyIDs.Add(EnemyID);
	LoadFoundVillagers(EnemyIDs);
}

TArray<UVillagerpediaButton*> UDDVillagerpediaWidget::FindVillagerpediaButtons(UWidget* Widget)
{
	TArray<UVillagerpediaButton*> Array;
	if (!Widget->IsA<UPanelWidget>()) {
		UE_LOG(LogTemp, Error, TEXT("Widget passed in is not a UPanelWidget, returning nothing"))
		return Array;
	}
	RecursiveFindButtons(Widget, Array);
	UE_LOG(LogTemp, Log, TEXT("Button amount: %d"), Array.Num());
	return Array;
}

void UDDVillagerpediaWidget::RecursiveFindButtons(UWidget* Widget, TArray<UVillagerpediaButton*>& Array)
{
	if (Widget->IsA<UVillagerpediaButton>()) {
		UVillagerpediaButton* Button = Cast<UVillagerpediaButton>(Widget);
		Array.Add(Button);
	}
	else if (Widget->IsA<UPanelWidget>()) {
		UPanelWidget* Panel = Cast<UPanelWidget>(Widget);
		TArray<UWidget*> ChildWidgets = Panel->GetAllChildren();
		for (UWidget* ChildWidget : ChildWidgets) {
			//Recursion in this day and age? How shocking!
			RecursiveFindButtons(ChildWidget, Array);
		}
	}
}

void UDDVillagerpediaWidget::SendNewVillagerAlert(const int32 EnemyID) 
{
	if (NewVillagerRef.IsValid()) {
		NewVillagerRef->ActivateNotification(EnemyID);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No reference found to NewVillagerAlertWidget"));
	}
}