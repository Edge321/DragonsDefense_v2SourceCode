// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDMainMenuWidget.h"
#include "Components/CanvasPanel.h"
#include "Kismet/KismetSystemLibrary.h"
//My classes
#include "Components/Button.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"

void UDDMainMenuWidget::Start()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GameStart();
	}
	SetVisibility(ESlateVisibility::Hidden);
}

void UDDMainMenuWidget::SetGameDifficulty() const
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->SetDifficulty(CurrentDifficultyChosen);
	}
}

void UDDMainMenuWidget::SetDifficultyChosen(const EDifficulty Difficulty) 
{
	CurrentDifficultyChosen = Difficulty;
}

void UDDMainMenuWidget::Quit() const
{
	UWorld* World = GetWorld();
	if (World) {
		UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Quit, false);
	}
}

void UDDMainMenuWidget::OpenVillagerpedia()
{
	OnMainMenuVillagerpedia.ExecuteIfBound(this);
}

void UDDMainMenuWidget::ExecuteToggleEnemyAttackBoxes(const bool AttackBoxVisibility) const
{
	OnToggleEnemyAttackBoxes.ExecuteIfBound(AttackBoxVisibility);
}

void UDDMainMenuWidget::SwitchDifficultyButton(UButton* DestButton) 
{
	if (CurrentDifficultyButton.IsValid()) {
		CurrentDifficultyButton->SetBackgroundColor(OriginalDifficultyButtonColor);
	}

	FLinearColor TempColor = DestButton->GetBackgroundColor();
	OriginalDifficultyButtonColor = TempColor;

	TempColor.R *= ActiveColorMultiplier;
	TempColor.G *= ActiveColorMultiplier;
	TempColor.B *= ActiveColorMultiplier;

	DestButton->SetBackgroundColor(TempColor);
	CurrentDifficultyButton = DestButton;
}

void UDDMainMenuWidget::ResetDifficultyButton() 
{
	if (CurrentDifficultyButton.IsValid()) {
		CurrentDifficultyButton->SetBackgroundColor(OriginalDifficultyButtonColor);
		CurrentDifficultyButton.Reset();
	}
}
