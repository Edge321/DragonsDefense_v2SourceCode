// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDMainMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"
//My classes
#include "Components/Button.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"

void UDDMainMenuWidget::InitializeWidget()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameStart.AddDynamic(this, &UDDMainMenuWidget::GameStartEventFunction);
	}
}

void UDDMainMenuWidget::Start()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GameStart();
	}
	SetVisibility(ESlateVisibility::Hidden);
}

void UDDMainMenuWidget::SetGameWave() const 
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GetEnemySpawner().SetCurrentWave(CurrentWaveChosen);
	}
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

void UDDMainMenuWidget::SetWaveChosen(const int32 Wave) 
{
	CurrentWaveChosen = Wave;
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

void UDDMainMenuWidget::SetIsWaveJumping(const bool WaveJumping) 
{
	bIsWaveJumping = WaveJumping;
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
	CurrentDifficultyButton = MakeWeakObjectPtr(DestButton);
}

void UDDMainMenuWidget::SwitchWaveButton(UButton* DestButton) 
{
	if (CurrentWaveButton.IsValid()) {
		CurrentWaveButton->SetBackgroundColor(OriginalWaveButtonColor);
	}

	FLinearColor TempColor = DestButton->GetBackgroundColor();
	OriginalWaveButtonColor = TempColor;

	TempColor.R *= ActiveColorMultiplier;
	TempColor.G *= ActiveColorMultiplier;
	TempColor.B *= ActiveColorMultiplier;

	DestButton->SetBackgroundColor(TempColor);
	CurrentWaveButton = MakeWeakObjectPtr(DestButton);
}

void UDDMainMenuWidget::ResetDifficultyButton() 
{
	if (CurrentDifficultyButton.IsValid()) {
		CurrentDifficultyButton->SetBackgroundColor(OriginalDifficultyButtonColor);
		CurrentDifficultyButton.Reset();
	}
}

void UDDMainMenuWidget::ResetWaveButton() 
{
	if (CurrentWaveButton.IsValid()) {
		CurrentWaveButton->SetBackgroundColor(OriginalWaveButtonColor);
		CurrentWaveButton.Reset();
	}
}

void UDDMainMenuWidget::GameStartEventFunction() 
{
	if (bIsWaveJumping) {
		OnWaveJumpChoice.ExecuteIfBound();
	}
}
